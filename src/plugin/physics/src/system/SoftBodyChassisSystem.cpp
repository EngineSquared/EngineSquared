#include "Physics.pch.hpp"

#include "system/SoftBodyChassisSystem.hpp"

#include "Logger.hpp"
#include "component/RigidBody.hpp"
#include "component/RigidBodyInternal.hpp"
#include "component/SoftBodyChassis.hpp"
#include "component/SoftBodyChassisInternal.hpp"
#include "component/Vehicle.hpp"
#include "component/VehicleInternal.hpp"
#include "exception/SoftBodyChassisError.hpp"
#include "resource/PhysicsManager.hpp"
#include "utils/JoltConversions.hpp"
#include "utils/Layers.hpp"
#include <fmt/format.h>
#include <set>
#include <unordered_map>

#include "Object.hpp"
#include "utils/MeshSimplifier.hpp"
#include "utils/MeshUtils.hpp"

#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyLock.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/ConvexHullShape.h>
#include <Jolt/Physics/SoftBody/SoftBodyCreationSettings.h>
#include <Jolt/Physics/SoftBody/SoftBodyMotionProperties.h>
#include <Jolt/Physics/SoftBody/SoftBodySharedSettings.h>

namespace Physics::System {

//=============================================================================
// Helper Functions
//=============================================================================

/**
 * @brief Structure holding deduplicated mesh data
 */
struct DeduplicatedMesh {
    std::vector<glm::vec3> vertices;
    std::vector<uint32_t> indices;
    std::vector<uint32_t> vertexMap;
};

/**
 * @brief Convert mesh to deduplicated indexed mesh
 */

/**
 * @brief Result from CreateChassisSharedSettings containing all necessary data
 */
struct ChassisSharedSettingsResult {
    JPH::Ref<JPH::SoftBodySharedSettings> settings;
    std::vector<uint32_t> vertexMap;        ///< Original mesh index -> Jolt vertex index
    std::vector<uint32_t> anchorIndices;    ///< Jolt indices of anchor vertices
    std::vector<glm::vec3> anchorPositions; ///< Local positions of anchors (scaled)
};

/**
 * @brief Create Jolt SoftBodySharedSettings for chassis with anchor vertex identification
 *
 * Creates a soft body that:
 * - Has all vertices connected by edge/shear/bend constraints
 * - Identifies "anchor" vertices near the bottom (attachment points to chassis)
 * - Returns anchor data for manual position control during sync
 *
 * The anchor vertices will be manually positioned each frame to follow the chassis,
 * while non-anchor vertices are free to deform under physics simulation.
 *
 * @param mesh The chassis mesh
 * @param settings The chassis settings
 * @param scale Scale to apply to vertices
 * @return ChassisSharedSettingsResult with settings, vertex map, and anchor data
 */
static ChassisSharedSettingsResult CreateChassisSharedSettings(const Object::Component::Mesh &mesh,
                                                               const Component::SoftBodyChassisSettings &settings,
                                                               const glm::vec3 &scale)
{
    ChassisSharedSettingsResult result;
    auto joltSettings = new JPH::SoftBodySharedSettings();

    auto dedupRes = Object::Utils::DeduplicateVertices(mesh);
    const auto &deduped_vertices = dedupRes.mesh.GetVertices();
    const auto &deduped_indices = dedupRes.mesh.GetIndices();

    Log::Info(fmt::format("SoftBodyChassis: {} original -> {} unique vertices", mesh.GetVertices().size(),
                          deduped_vertices.size()));

    // Compute bounding box to identify anchor vertices (near bottom)
    glm::vec3 minBound(FLT_MAX), maxBound(-FLT_MAX);
    for (const auto &v : deduped_vertices)
    {
        glm::vec3 scaledV = v * scale;
        minBound = glm::min(minBound, scaledV);
        maxBound = glm::max(maxBound, scaledV);
    }

    float height = maxBound.y - minBound.y;
    float anchorThreshold = minBound.y + height * 0.15f; // Bottom 15% of vertices are anchors

    joltSettings->mVertices.reserve(deduped_vertices.size());
    for (size_t i = 0; i < deduped_vertices.size(); ++i)
    {
        const auto &v = deduped_vertices[i];
        JPH::SoftBodySharedSettings::Vertex vertex;
        glm::vec3 scaledPos = v * scale;
        vertex.mPosition = JPH::Float3(scaledPos.x, scaledPos.y, scaledPos.z);
        vertex.mVelocity = JPH::Float3(0.0f, 0.0f, 0.0f);
        vertex.mInvMass = 1.0f; // All vertices dynamic, we control anchors manually

        joltSettings->mVertices.emplace_back(vertex);

        // Identify anchor vertices (near the bottom of the mesh)
        if (scaledPos.y <= anchorThreshold)
        {
            result.anchorIndices.push_back(static_cast<uint32_t>(i));
            result.anchorPositions.push_back(scaledPos);
        }
    }

    Log::Info(fmt::format("SoftBodyChassis: Identified {} anchor vertices (bottom 15%)", result.anchorIndices.size()));

    if (!deduped_indices.empty())
    {
        joltSettings->mFaces.reserve(deduped_indices.size() / 3u);
        for (size_t i = 0u; i + 2u < deduped_indices.size(); i += 3u)
        {
            uint32_t idx0 = deduped_indices[i];
            uint32_t idx1 = deduped_indices[i + 1u];
            uint32_t idx2 = deduped_indices[i + 2u];

            if (idx0 == idx1 || idx1 == idx2 || idx0 == idx2)
                continue;
            if (idx0 >= deduped_vertices.size() || idx1 >= deduped_vertices.size() || idx2 >= deduped_vertices.size())
                continue;

            joltSettings->mFaces.emplace_back(JPH::SoftBodySharedSettings::Face(idx0, idx1, idx2, 0u));
        }

        // Use soft compliance to allow significant deformation
        float edgeCompliance = (1.0f - settings.stiffness) * 0.01f;
        float shearCompliance = (1.0f - settings.stiffness) * 0.02f;
        float bendCompliance = (1.0f - settings.stiffness) * 1.0f;

        JPH::SoftBodySharedSettings::VertexAttributes attributes(edgeCompliance, shearCompliance, bendCompliance);
        joltSettings->CreateConstraints(&attributes, 1, JPH::SoftBodySharedSettings::EBendType::Distance);
    }

    joltSettings->Optimize();

    result.settings = joltSettings;
    result.vertexMap = std::move(dedupRes.vertexMap);

    return result;
}

//=============================================================================
// Soft Body Chassis Lifecycle Handlers
//=============================================================================

static void OnSoftBodyChassisConstruct(Engine::Core::Registry &registry, Engine::EntityId entityId)
{
    try
    {
        auto *corePtr = registry.ctx().get<Engine::Core *>();
        if (!corePtr)
        {
            Log::Error("SoftBodyChassis: Engine::Core not available");
            return;
        }
        auto &core = *corePtr;
        Engine::Entity entity{core, entityId};

        auto &physicsManager = core.GetResource<Resource::PhysicsManager>();
        if (!physicsManager.IsPhysicsActivated())
        {
            Log::Error("SoftBodyChassis: Physics system not activated");
            return;
        }

        auto &chassisComp = entity.GetComponents<Component::SoftBodyChassis>();
        const auto &settings = chassisComp.settings;

        auto *mesh = entity.TryGetComponent<Object::Component::Mesh>();
        if (!mesh)
        {
            Log::Error("SoftBodyChassis: No Mesh component found");
            return;
        }

        auto *transform = entity.TryGetComponent<Object::Component::Transform>();
        if (!transform)
        {
            Log::Error("SoftBodyChassis: No Transform component found");
            return;
        }

        const auto &meshVertices = mesh->GetVertices();
        if (meshVertices.empty())
        {
            Log::Error("SoftBodyChassis: Mesh has no vertices");
            return;
        }

        Object::Component::Mesh workingMesh = *mesh;
        std::vector<uint32_t> simplificationMap;
        bool wasSimplified = false;
        auto originalVertexCount = static_cast<uint32_t>(meshVertices.size());

        if (meshVertices.size() > settings.maxVertices)
        {
            if (settings.autoSimplify)
            {
                Log::Info(fmt::format("SoftBodyChassis: Simplifying mesh from {} to max {} vertices",
                                      meshVertices.size(), settings.maxVertices));

                auto simplifySettings = Object::Utils::SimplificationSettings::ForVehicleChassis();
                simplifySettings.targetVertexCount = settings.maxVertices;

                auto result = Object::Utils::SimplifyMesh(*mesh, simplifySettings);

                if (result.wasSimplified && result.simplifiedVertexCount <= settings.maxVertices)
                {
                    workingMesh = std::move(result.mesh);
                    simplificationMap = std::move(result.vertexMap);
                    wasSimplified = true;
                    Log::Info(fmt::format("SoftBodyChassis: Simplified to {} vertices", result.simplifiedVertexCount));
                }
                else if (settings.fallbackToRigidBody)
                {
                    Log::Warn("SoftBodyChassis: Simplification insufficient, falling back to RigidBody");
                    chassisComp.isActive = false;
                    return;
                }
            }
            else if (settings.fallbackToRigidBody)
            {
                Log::Warn(fmt::format("SoftBodyChassis: Mesh too complex ({} vertices), falling back to RigidBody",
                                      meshVertices.size()));
                chassisComp.isActive = false;
                return;
            }
        }

        glm::vec3 position = transform->GetPosition();
        glm::quat rotation = transform->GetRotation();
        glm::vec3 scale = transform->GetScale();

        auto chassisResult = CreateChassisSharedSettings(workingMesh, settings, scale);

        JPH::SoftBodyCreationSettings softBodySettings(chassisResult.settings, Utils::ToJoltRVec3(position),
                                                       Utils::ToJoltQuat(rotation), Utils::Layers::MOVING);

        softBodySettings.mNumIterations = settings.solverIterations;
        softBodySettings.mLinearDamping = settings.linearDamping;
        softBodySettings.mFriction = settings.friction;
        softBodySettings.mVertexRadius = settings.vertexRadius;
        softBodySettings.mAllowSleeping = false;

        auto &bodyInterface = physicsManager.GetBodyInterface();
        JPH::Body *softBody = bodyInterface.CreateSoftBody(softBodySettings);

        if (!softBody)
        {
            Log::Error("SoftBodyChassis: Failed to create soft body");
            return;
        }

        JPH::BodyID softBodyID = softBody->GetID();
        bodyInterface.AddBody(softBodyID, JPH::EActivation::Activate);

        Component::SoftBodyChassisInternal internal;
        internal.softBodyID = softBodyID;
        internal.initialScale = scale;
        internal.isInitialized = true;
        internal.wasSimplified = wasSimplified;
        internal.originalVertexCount = originalVertexCount;
        internal.simplifiedVertexCount = static_cast<uint32_t>(workingMesh.GetVertices().size());
        internal.hardSkinNextFrame = true;

        // Store anchor data for manual position control
        internal.anchorVertexIndices = std::move(chassisResult.anchorIndices);
        internal.anchorLocalPositions = std::move(chassisResult.anchorPositions);
        internal.anchorsInitialized = true;

        if (wasSimplified)
        {
            std::vector<uint32_t> composedMap(simplificationMap.size());
            for (size_t i = 0u; i < simplificationMap.size(); ++i)
            {
                uint32_t simpIdx = simplificationMap[i];
                composedMap[i] = (simpIdx < chassisResult.vertexMap.size()) ? chassisResult.vertexMap[simpIdx] : 0u;
            }
            internal.vertexMap = std::move(composedMap);
        }
        else
        {
            internal.vertexMap = std::move(chassisResult.vertexMap);
        }

        registry.emplace<Component::SoftBodyChassisInternal>(entity, std::move(internal));

        chassisComp.isActive = true;

        Log::Info(fmt::format("SoftBodyChassis: Created for entity {} ({} vertices, simplified: {})", entity,
                              workingMesh.GetVertices().size(), wasSimplified));
    }
    catch (const Exception::SoftBodyChassisError &e)
    {
        Log::Error(fmt::format("SoftBodyChassis: Runtime error during creation: {}", e.what()));
    }
    catch (const std::bad_alloc &e)
    {
        Log::Error(fmt::format("SoftBodyChassis: Allocation failed during creation: {}", e.what()));
    }
}

static void OnSoftBodyChassisDestroy(Engine::Core::Registry &registry, Engine::EntityId entityId)
{
    auto *corePtr = registry.ctx().get<Engine::Core *>();
    if (!corePtr)
        return;

    auto &core = *corePtr;
    auto &physicsManager = core.GetResource<Resource::PhysicsManager>();

    if (!physicsManager.IsPhysicsActivated())
        return;

    auto *internal = registry.try_get<Component::SoftBodyChassisInternal>(entityId);
    if (!internal)
        return;

    auto &bodyInterface = physicsManager.GetBodyInterface();

    if (internal->HasValidSoftBody())
    {
        bodyInterface.RemoveBody(internal->softBodyID);
        bodyInterface.DestroyBody(internal->softBodyID);
        Log::Debug(fmt::format("SoftBodyChassis: Destroyed soft body for entity {}", entityId));
    }
}

//=============================================================================
// System Functions
//=============================================================================

void InitSoftBodyChassisSystem(Engine::Core &core)
{
    auto &registry = core.GetRegistry();

    if (!registry.ctx().contains<Engine::Core *>())
    {
        registry.ctx().emplace<Engine::Core *>(&core);
    }

    registry.on_construct<Component::SoftBodyChassis>().connect<&OnSoftBodyChassisConstruct>();
    registry.on_destroy<Component::SoftBodyChassis>().connect<&OnSoftBodyChassisDestroy>();

    Log::Info("SoftBodyChassisSystem initialized");
}

void ShutdownSoftBodyChassisSystem(Engine::Core &core)
{
    auto &registry = core.GetRegistry();

    registry.on_construct<Component::SoftBodyChassis>().disconnect<&OnSoftBodyChassisConstruct>();
    registry.on_destroy<Component::SoftBodyChassis>().disconnect<&OnSoftBodyChassisDestroy>();

    Log::Info("SoftBodyChassisSystem shutdown");
}

void SyncSoftBodyChassisWithSkeleton(Engine::Core &core)
{
    auto &registry = core.GetRegistry();
    auto &physicsManager = core.GetResource<Resource::PhysicsManager>();

    if (!physicsManager.IsPhysicsActivated())
        return;

    auto view =
        registry.view<Component::SoftBodyChassis, Component::SoftBodyChassisInternal, Object::Component::Transform>();

    auto &bodyInterface = physicsManager.GetBodyInterface();

    for (auto entity : view)
    {
        auto &chassis = view.get<Component::SoftBodyChassis>(entity);
        auto &internal = view.get<Component::SoftBodyChassisInternal>(entity);
        auto &transform = view.get<Object::Component::Transform>(entity);

        if (!chassis.isActive || !internal.IsValid() || !internal.anchorsInitialized)
            continue;

        glm::vec3 position = transform.GetPosition();
        glm::quat rotation = transform.GetRotation();

        // Lock the soft body for writing
        JPH::BodyLockWrite lock(physicsManager.GetPhysicsSystem().GetBodyLockInterface(), internal.softBodyID);
        if (!lock.Succeeded())
            continue;

        JPH::Body &body = lock.GetBody();
        if (!body.IsSoftBody())
            continue;

        auto *motionProps = static_cast<JPH::SoftBodyMotionProperties *>(body.GetMotionProperties());
        auto &joltVertices = motionProps->GetVertices();

        // On first frame or reset, teleport the entire soft body to match chassis
        if (internal.hardSkinNextFrame)
        {
            // Set the body position and rotation to match chassis
            body.SetPositionAndRotationInternal(Utils::ToJoltRVec3(position), Utils::ToJoltQuat(rotation));
            internal.hardSkinNextFrame = false;
        }
        else
        {
            // Normal frame: Update position/rotation
            body.SetPositionAndRotationInternal(Utils::ToJoltRVec3(position), Utils::ToJoltQuat(rotation));
        }

        // Now pin the anchor vertices to their original local positions
        // This keeps the bottom of the mesh attached to the chassis
        // while allowing the rest to deform from collisions
        for (size_t i = 0; i < internal.anchorVertexIndices.size(); ++i)
        {
            uint32_t joltVertexIdx = internal.anchorVertexIndices[i];
            if (joltVertexIdx >= joltVertices.size())
                continue;

            const glm::vec3 &localPos = internal.anchorLocalPositions[i];

            // Set anchor vertex to its original local position (relative to body COM)
            joltVertices[joltVertexIdx].mPosition = JPH::Vec3(localPos.x, localPos.y, localPos.z);
            // Zero velocity to prevent oscillation
            joltVertices[joltVertexIdx].mVelocity = JPH::Vec3::sZero();
        }
    }
}

void SyncSoftBodyChassisMesh(Engine::Core &core)
{
    auto &registry = core.GetRegistry();
    auto &physicsManager = core.GetResource<Resource::PhysicsManager>();

    if (!physicsManager.IsPhysicsActivated())
        return;

    auto view = registry.view<Component::SoftBodyChassis, Component::SoftBodyChassisInternal, Object::Component::Mesh,
                              Object::Component::Transform>();

    auto &bodyLockInterface = physicsManager.GetPhysicsSystem().GetBodyLockInterface();

    for (auto [entity, chassis, internal, mesh, transform] : view.each())
    {
        if (!chassis.isActive || !internal.HasValidSoftBody())
            continue;

        JPH::BodyLockRead lock(bodyLockInterface, internal.softBodyID);
        if (!lock.Succeeded())
            continue;

        const JPH::Body &body = lock.GetBody();
        if (!body.IsSoftBody())
            continue;

        const auto *motionProps = static_cast<const JPH::SoftBodyMotionProperties *>(body.GetMotionProperties());
        const auto &joltVertices = motionProps->GetVertices();

        // Use safe inverse scale
        glm::vec3 safeScale = internal.initialScale;
        constexpr float epsilon = 1e-6f;
        if (std::abs(safeScale.x) < epsilon)
            safeScale.x = 1.0f;
        if (std::abs(safeScale.y) < epsilon)
            safeScale.y = 1.0f;
        if (std::abs(safeScale.z) < epsilon)
            safeScale.z = 1.0f;
        glm::vec3 invScale = 1.0f / safeScale;

        auto &meshVertices = mesh.GetVertices();
        const auto &vertexMap = internal.vertexMap;

        // Jolt SoftBody vertices are in LOCAL space relative to the body's center of mass
        // We just need to apply inverse scale to convert back to original mesh space
        for (size_t i = 0u; i < meshVertices.size() && i < vertexMap.size(); ++i)
        {
            uint32_t joltIdx = vertexMap[i];
            if (joltIdx < joltVertices.size())
            {
                // mPosition is already in local space relative to center of mass
                glm::vec3 localPos = Utils::FromJoltVec3(joltVertices[joltIdx].mPosition);
                // Apply inverse scale to convert from Jolt space back to mesh space
                localPos *= invScale;
                mesh.SetVertexAt(i, localPos);
            }
        }

        Object::Utils::RecalculateNormals(mesh);
    }
}

} // namespace Physics::System
