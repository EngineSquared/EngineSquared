#include "Physics.pch.hpp"

#include "system/SoftBodyChassisSystem.hpp"

#include "Logger.hpp"
#include "component/RigidBody.hpp"
#include "component/RigidBodyInternal.hpp"
#include "component/SoftBodyChassis.hpp"
#include "component/SoftBodyChassisInternal.hpp"
#include "component/Vehicle.hpp"
#include "component/VehicleInternal.hpp"
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
 * @brief Create Jolt SoftBodySharedSettings with skinned constraints for chassis
 *
 * @param mesh The chassis mesh
 * @param settings The chassis settings
 * @param scale Scale to apply to vertices
 * @return Pair of shared settings and vertex map
 */
static std::pair<JPH::Ref<JPH::SoftBodySharedSettings>, std::vector<uint32_t>>
CreateChassisSharedSettings(const Object::Component::Mesh &mesh, const Component::SoftBodyChassisSettings &settings,
                            const glm::vec3 &scale)
{
    auto joltSettings = new JPH::SoftBodySharedSettings();

    auto dedupRes = Object::Utils::DeduplicateVertices(mesh);
    const auto &deduped_vertices = dedupRes.mesh.GetVertices();
    const auto &deduped_indices = dedupRes.mesh.GetIndices();

    Log::Info(fmt::format("SoftBodyChassis: {} original -> {} unique vertices", mesh.GetVertices().size(),
                          deduped_vertices.size()));

    joltSettings->mVertices.reserve(deduped_vertices.size());
    for (const auto &v : deduped_vertices)
    {
        JPH::SoftBodySharedSettings::Vertex vertex;
        glm::vec3 scaledPos = v * scale;
        vertex.mPosition = JPH::Float3(scaledPos.x, scaledPos.y, scaledPos.z);
        vertex.mVelocity = JPH::Float3(0, 0, 0);
        vertex.mInvMass = 1.0f;
        joltSettings->mVertices.emplace_back(vertex);
    }

    if (!deduped_indices.empty())
    {
        joltSettings->mFaces.reserve(deduped_indices.size() / 3);
        for (size_t i = 0; i + 2 < deduped_indices.size(); i += 3)
        {
            uint32_t idx0 = deduped_indices[i];
            uint32_t idx1 = deduped_indices[i + 1];
            uint32_t idx2 = deduped_indices[i + 2];

            if (idx0 == idx1 || idx1 == idx2 || idx0 == idx2)
                continue;
            if (idx0 >= deduped_vertices.size() || idx1 >= deduped_vertices.size() || idx2 >= deduped_vertices.size())
                continue;

            joltSettings->mFaces.emplace_back(JPH::SoftBodySharedSettings::Face(idx0, idx1, idx2, 0));
        }

        float edgeCompliance = (1.0f - settings.stiffness) * 0.001f;
        float shearCompliance = (1.0f - settings.stiffness) * 0.002f;
        float bendCompliance = (1.0f - settings.stiffness) * 0.1f;

        JPH::SoftBodySharedSettings::VertexAttributes attributes(edgeCompliance, shearCompliance, bendCompliance);
        joltSettings->CreateConstraints(&attributes, 1, JPH::SoftBodySharedSettings::EBendType::Distance);
    }

    joltSettings->mInvBindMatrices.emplace_back(JPH::SoftBodySharedSettings::InvBind(0, JPH::Mat44::sIdentity()));

    for (uint32_t i = 0; i < joltSettings->mVertices.size(); ++i)
    {
        JPH::SoftBodySharedSettings::Skinned skinned;
        skinned.mVertex = i;
        skinned.mMaxDistance = settings.maxDeformation;
        skinned.mBackStopDistance = settings.backStopDistance;
        skinned.mBackStopRadius = settings.backStopRadius;

        skinned.mWeights[0] = JPH::SoftBodySharedSettings::SkinWeight(0, 1.0f);
        if (JPH::SoftBodySharedSettings::Skinned::cMaxSkinWeights > 1)
        {
            skinned.mWeights[1] = JPH::SoftBodySharedSettings::SkinWeight(0, 0.0f);
        }

        joltSettings->mSkinnedConstraints.emplace_back(skinned);
    }

    joltSettings->CalculateSkinnedConstraintNormals();

    joltSettings->Optimize();

    return {joltSettings, std::move(dedupRes.vertexMap)};
}

/**
 * @brief Create invisible skeleton RigidBody for VehicleConstraint
 */
static JPH::BodyID CreateSkeletonBody(Resource::PhysicsManager &physicsManager, const glm::vec3 &position,
                                      const glm::quat &rotation, const glm::vec3 &halfExtents, float mass)
{
    JPH::BoxShapeSettings shapeSettings(JPH::Vec3(halfExtents.x, halfExtents.y, halfExtents.z));
    auto shapeResult = shapeSettings.Create();

    if (shapeResult.HasError())
    {
        Log::Error(fmt::format("SoftBodyChassis: Failed to create skeleton shape: {}", shapeResult.GetError().c_str()));
        return JPH::BodyID();
    }

    JPH::BodyCreationSettings bodySettings(shapeResult.Get(), Utils::ToJoltRVec3(position), Utils::ToJoltQuat(rotation),
                                           JPH::EMotionType::Dynamic, Utils::Layers::MOVING);

    bodySettings.mOverrideMassProperties = JPH::EOverrideMassProperties::CalculateInertia;
    bodySettings.mMassPropertiesOverride.mMass = mass;
    bodySettings.mLinearDamping = 0.1f;
    bodySettings.mAngularDamping = 0.1f;
    bodySettings.mAllowSleeping = false;

    auto &bodyInterface = physicsManager.GetBodyInterface();
    JPH::Body *body = bodyInterface.CreateBody(bodySettings);

    if (!body)
    {
        Log::Error("SoftBodyChassis: Failed to create skeleton body");
        return JPH::BodyID();
    }

    return body->GetID();
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

        glm::vec3 minBound(FLT_MAX);
        glm::vec3 maxBound(-FLT_MAX);
        for (const auto &v : workingMesh.GetVertices())
        {
            glm::vec3 scaledV = v * scale;
            minBound = glm::min(minBound, scaledV);
            maxBound = glm::max(maxBound, scaledV);
        }
        glm::vec3 halfExtents = (maxBound - minBound) * 0.5f;

        auto [sharedSettings, vertexMap] = CreateChassisSharedSettings(workingMesh, settings, scale);

        JPH::SoftBodyCreationSettings softBodySettings(sharedSettings, Utils::ToJoltRVec3(position),
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
        internal.vertexMap = wasSimplified ? simplificationMap : std::move(vertexMap);
        internal.initialScale = scale;
        internal.isInitialized = true;
        internal.wasSimplified = wasSimplified;
        internal.originalVertexCount = originalVertexCount;
        internal.simplifiedVertexCount = static_cast<uint32_t>(workingMesh.GetVertices().size());
        internal.hardSkinNextFrame = true;

        registry.emplace<Component::SoftBodyChassisInternal>(entity, std::move(internal));

        chassisComp.isActive = true;

        Log::Info(fmt::format("SoftBodyChassis: Created for entity {} ({} vertices, simplified: {})", entity,
                              workingMesh.GetVertices().size(), wasSimplified));
    }
    catch (const std::runtime_error &e)
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
    auto &bodyLockInterface = physicsManager.GetPhysicsSystem().GetBodyLockInterface();

    for (auto [entity, chassis, internal, transform] : view.each())
    {
        if (!chassis.isActive || !internal.IsValid())
            continue;

        glm::vec3 position = transform.GetPosition();
        glm::quat rotation = transform.GetRotation();

        JPH::Mat44 jointMatrix =
            JPH::Mat44::sRotationTranslation(Utils::ToJoltQuat(rotation), Utils::ToJoltVec3(position));

        internal.UpdateJointMatrix(jointMatrix);

        JPH::BodyLockWrite lock(bodyLockInterface, internal.softBodyID);
        if (!lock.Succeeded())
            continue;

        JPH::Body &body = lock.GetBody();
        if (!body.IsSoftBody())
            continue;

        auto *motionProps = static_cast<JPH::SoftBodyMotionProperties *>(body.GetMotionProperties());

        JPH::RMat44 comTransform = body.GetCenterOfMassTransform();

        motionProps->SkinVertices(comTransform, internal.jointMatrices.data(), internal.numJoints,
                                  internal.hardSkinNextFrame, *physicsManager.GetTempAllocator());

        internal.hardSkinNextFrame = false;
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

        glm::vec3 invScale = 1.0f / internal.initialScale;

        auto &meshVertices = mesh.GetVertices();
        const auto &vertexMap = internal.vertexMap;

        glm::vec3 bodyPos = Utils::FromJoltRVec3(body.GetPosition());
        glm::quat bodyRot = Utils::FromJoltQuat(body.GetRotation());
        glm::quat invBodyRot = glm::inverse(bodyRot);

        for (size_t i = 0; i < meshVertices.size() && i < vertexMap.size(); ++i)
        {
            uint32_t joltIdx = vertexMap[i];
            if (joltIdx < joltVertices.size())
            {
                glm::vec3 worldPos = Utils::FromJoltVec3(joltVertices[joltIdx].mPosition);
                glm::vec3 localPos = invBodyRot * (worldPos - bodyPos);
                localPos *= invScale;
                mesh.SetVertexAt(i, localPos);
            }
        }

        Object::Utils::RecalculateNormals(mesh);
    }
}

} // namespace Physics::System
