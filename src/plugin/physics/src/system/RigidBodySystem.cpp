#include "Physics.pch.hpp"

#include "system/RigidBodySystem.hpp"

#include "Logger.hpp"
#include "component/BoxCollider.hpp"
#include "component/CapsuleCollider.hpp"
#include "component/ConvexHullMeshCollider.hpp"
#include "component/MeshCollider.hpp"
#include "component/RigidBody.hpp"
#include "component/RigidBodyInternal.hpp"
#include "component/SphereCollider.hpp"
#include "exception/RigidBodyError.hpp"
#include "resource/BodyEntityMap.hpp"
#include "resource/PhysicsManager.hpp"
#include "utils/JoltConversions.hpp"
#include <fmt/format.h>

#include "Object.hpp"

#include <Jolt/Geometry/IndexedTriangle.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/ConvexHullShape.h>
#include <Jolt/Physics/Collision/Shape/MeshShape.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>

namespace Physics::System {

//=============================================================================
// Collider shape creation
//=============================================================================

/**
 * @brief Create a ConvexHullShape from mesh vertices
 * @param mesh The mesh component containing vertices
 * @param meshCollider Pointer to ConvexHullMeshCollider settings (nullable). If null, default settings are used.
 * @param scale Scale to apply to the mesh vertices (from Transform component)
 * @return RefConst to the created shape, or nullptr on failure
 */
static JPH::RefConst<JPH::Shape> CreateConvexHullFromMesh(const Object::Component::Mesh &mesh,
                                                          const Component::ConvexHullMeshCollider *meshCollider,
                                                          const glm::vec3 &scale)
{
    const auto &vertices = mesh.GetVertices();

    if (vertices.empty())
    {
        Log::Warn("ConvexHullMeshCollider: Mesh has no vertices, cannot create convex hull");
        return nullptr;
    }

    JPH::Array<JPH::Vec3> joltPoints;
    joltPoints.reserve(vertices.size());

    for (const auto &vertex : vertices)
    {
        glm::vec3 scaledVertex = vertex * scale;
        joltPoints.push_back(JPH::Vec3(scaledVertex.x, scaledVertex.y, scaledVertex.z));
    }

    float maxConvexRadius =
        meshCollider ? meshCollider->maxConvexRadius : Component::ConvexHullMeshCollider{}.maxConvexRadius;
    JPH::ConvexHullShapeSettings settings(joltPoints, maxConvexRadius);

    JPH::ShapeSettings::ShapeResult result = settings.Create();
    if (!result.IsValid())
    {
        Log::Error(
            fmt::format("ConvexHullMeshCollider: Failed to create convex hull shape: {}", result.GetError().c_str()));
        return nullptr;
    }

    return result.Get();
}

/**
 * @brief Create a MeshShape from mesh vertices and indices
 * @param mesh The mesh component containing vertices and indices
 * @param meshCollider Pointer to MeshCollider settings (nullable)
 * @param scale Scale to apply to the mesh vertices (from Transform component)
 * @return RefConst to the created shape, or nullptr on failure
 */
static JPH::RefConst<JPH::Shape> CreateMeshShapeFromMesh(const Object::Component::Mesh &mesh,
                                                         const Component::MeshCollider *meshCollider,
                                                         const glm::vec3 &scale)
{
    const auto &vertices = mesh.GetVertices();
    const auto &indices = mesh.GetIndices();

    if (vertices.empty() || indices.empty())
    {
        Log::Warn("MeshCollider: Mesh has no vertices or indices, cannot create mesh shape");
        return nullptr;
    }

    JPH::VertexList joltVertices;
    joltVertices.reserve(vertices.size());
    for (const auto &vertex : vertices)
    {
        glm::vec3 scaledVertex = vertex * scale;
        joltVertices.push_back(JPH::Float3(scaledVertex.x, scaledVertex.y, scaledVertex.z));
    }

    JPH::IndexedTriangleList joltTriangles;
    joltTriangles.reserve(indices.size() / 3);

    for (size_t i = 0; i < indices.size(); i += 3)
    {
        if (i + 2 >= indices.size())
            break;

        joltTriangles.push_back(JPH::IndexedTriangle(indices[i], indices[i + 1], indices[i + 2], 0));
    }

    JPH::MeshShapeSettings settings(joltVertices, joltTriangles);

    if (meshCollider)
    {
        settings.mActiveEdgeCosThresholdAngle = meshCollider->activeEdgeCosThresholdAngle;
    }

    JPH::ShapeSettings::ShapeResult result = settings.Create();
    if (!result.IsValid())
    {
        Log::Error(fmt::format("MeshCollider: Failed to create mesh shape: {}", result.GetError().c_str()));
        return nullptr;
    }

    return result.Get();
}

/**
 * @brief Create a Jolt shape from collider components
 * @return Shared pointer to shape, or nullptr if no collider found
 *
 * @note Priority order when multiple colliders exist:
 * 1. SphereCollider
 * 2. CapsuleCollider
 * 3. BoxCollider
 * 4. MeshCollider (requires Object::Mesh component)
 *
 * @note If no collider is found, it will default to the MeshCollider with default settings, which can be pretty heavy.
 * Make sure to always use the most appropriate colliders for RigidBodies.
 */
static JPH::RefConst<JPH::Shape> CreateShapeFromColliders(Engine::Core::Registry &registry, // NOSONAR
                                                          Engine::EntityId entity)          // NOSONAR
{
    if (auto *sphereCollider = registry.try_get<Component::SphereCollider>(entity))
    {
        if (!sphereCollider->IsValid())
        {
            Log::Warn("SphereCollider: Invalid radius, using default 0.5");
            return new JPH::SphereShape(0.5f);
        }

        auto *baseShape = new JPH::SphereShape(sphereCollider->radius);
        if (sphereCollider->offset != glm::vec3{0.0f, 0.0f, 0.0f})
            return new JPH::RotatedTranslatedShape(Utils::ToJoltVec3(sphereCollider->offset), JPH::Quat::sIdentity(),
                                                   baseShape);
        return baseShape;
    }

    if (auto *capsuleCollider = registry.try_get<Component::CapsuleCollider>(entity))
    {
        if (!capsuleCollider->IsValid())
        {
            Log::Warn("CapsuleCollider: Invalid dimensions, using default");
            return new JPH::CapsuleShape(0.5f, 0.25f);
        }

        auto *baseShape = new JPH::CapsuleShape(capsuleCollider->halfHeight, capsuleCollider->radius);
        if (capsuleCollider->offset != glm::vec3{0.0f, 0.0f, 0.0f})
            return new JPH::RotatedTranslatedShape(Utils::ToJoltVec3(capsuleCollider->offset), JPH::Quat::sIdentity(),
                                                   baseShape);
        return baseShape;
    }

    if (auto *boxCollider = registry.try_get<Component::BoxCollider>(entity))
    {
        auto *baseShape = new JPH::BoxShape(Utils::ToJoltVec3(boxCollider->halfExtents), boxCollider->convexRadius);
        if (boxCollider->offset != glm::vec3{0.0f, 0.0f, 0.0f})
            return new JPH::RotatedTranslatedShape(Utils::ToJoltVec3(boxCollider->offset), JPH::Quat::sIdentity(),
                                                   baseShape);
        return baseShape;
    }

    if (auto *convexHullCollider = registry.try_get<Component::ConvexHullMeshCollider>(entity))
    {
        const Object::Component::Mesh *mesh = nullptr;

        if (convexHullCollider->mesh.has_value())
        {
            mesh = &convexHullCollider->mesh.value();
        }
        else
        {
            mesh = registry.try_get<Object::Component::Mesh>(entity);
        }

        if (!mesh)
        {
            Log::Warn("ConvexHullMeshCollider: trying to create shape without mesh data (no embedded mesh or "
                      "Object::Mesh component)");
            return nullptr;
        }

        glm::vec3 scale(1.0f, 1.0f, 1.0f);
        if (auto *transform = registry.try_get<Object::Component::Transform>(entity))
        {
            scale = transform->GetScale();
        }

        return CreateConvexHullFromMesh(*mesh, convexHullCollider, scale);
    }

    auto *meshCollider = registry.try_get<Component::MeshCollider>(entity);
    const Object::Component::Mesh *mesh = nullptr;

    if (meshCollider && meshCollider->mesh.has_value())
    {
        mesh = &meshCollider->mesh.value();
    }
    else
    {
        mesh = registry.try_get<Object::Component::Mesh>(entity);
    }

    if (!mesh)
    {
        Log::Warn(
            "MeshCollider: trying to create shape without mesh data (no embedded mesh or Object::Mesh component)");
        return nullptr;
    }

    glm::vec3 scale(1.0f, 1.0f, 1.0f);
    if (auto *transform = registry.try_get<Object::Component::Transform>(entity))
    {
        scale = transform->GetScale();
    }

    return CreateMeshShapeFromMesh(*mesh, meshCollider, scale);
}

//=============================================================================
// Entt hook callbacks
//=============================================================================

/**
 * @brief Called when RigidBody component is added to an entity
 *
 * @note This creates:
 * 1. Default collider if no collider exists
 * 2. Jolt physics body with all properties
 * 3. RigidBodyInternal component with BodyID
 */
static void OnRigidBodyConstruct(Engine::Core::Registry &registry, Engine::EntityId entityId)
{
    try
    {
        auto core = registry.ctx().get<Engine::Core *>();
        Engine::Entity entity{*core, entityId};

        auto &physicsManager = core->GetResource<Resource::PhysicsManager>();
        if (!physicsManager.IsPhysicsActivated())
        {
            Log::Error("Cannot create RigidBody: Physics system not activated");
            return;
        }

        auto &rigidBody = entity.GetComponents<Component::RigidBody>();

        auto *transform = entity.TryGetComponent<Object::Component::Transform>();
        if (!transform)
        {
            Log::Warn("RigidBody added to entity without Transform - creating default Transform");
            transform = &entity.AddComponent<Object::Component::Transform>();
        }

        auto shape = CreateShapeFromColliders(registry, entity);
        if (!shape)
        {
            Log::Error("Failed to create collider shape for RigidBody");
            return;
        }

        JPH::BodyCreationSettings bodySettings(shape, Utils::ToJoltVec3(transform->GetPosition()),
                                               Utils::ToJoltQuat(transform->GetRotation()), rigidBody.motionType,
                                               rigidBody.objectLayer);

        bodySettings.mUserData = static_cast<uint64_t>(entity.Id());
        bodySettings.mFriction = rigidBody.friction;
        bodySettings.mRestitution = rigidBody.restitution;
        bodySettings.mLinearDamping = rigidBody.linearDamping;
        bodySettings.mAngularDamping = rigidBody.angularDamping;
        bodySettings.mGravityFactor = rigidBody.gravityFactor;
        bodySettings.mAllowSleeping = rigidBody.allowSleeping;

        if (rigidBody.motionType == Component::MotionType::Dynamic)
        {
            bodySettings.mOverrideMassProperties = JPH::EOverrideMassProperties::CalculateInertia;
            bodySettings.mMassPropertiesOverride.mMass = rigidBody.mass;
        }

        auto &bodyInterface = physicsManager.GetBodyInterface();
        JPH::Body *body = bodyInterface.CreateBody(bodySettings);

        if (!body)
        {
            Log::Error("Failed to create Jolt physics body");
            return;
        }

        JPH::BodyID bodyID = body->GetID();
        bodyInterface.AddBody(bodyID, rigidBody.activation);

        entity.AddComponent<Component::RigidBodyInternal>(bodyID);
        core->GetResource<Resource::BodyEntityMap>().Add(entity, bodyID);

        Log::Debug(
            fmt::format("Created RigidBody for entity {} with BodyID {}", entity, bodyID.GetIndexAndSequenceNumber()));
    }
    catch (const Exception::RigidBodyError &e)
    {
        Log::Error(fmt::format("RigidBodyError in OnRigidBodyConstruct: {}", e.what()));
    }
}

static void OnRigidBodyInternalConstruct(Engine::Core::Registry &registry, Engine::EntityId entityId)
{
    auto &core = registry.ctx().get<Engine::Core *>();
    Engine::Entity entity{*core, entityId};

    auto internalComponent = entity.GetComponents<Component::RigidBodyInternal>();

    if (!internalComponent.IsValid())
    {
        Log::Warn("RigidBodyInternal has invalid BodyID, skipping body addition to BodyEntityMap");
        return;
    }

    core->GetResource<Resource::BodyEntityMap>().Add(entity, internalComponent.bodyID);
}

/**
 * @brief Called when RigidBody component is removed from an entity
 *
 * @note This destroys:
 * 1. Jolt physics body
 * 2. RigidBodyInternal component
 *
 * @note Colliders are NOT automatically removed (user may want to keep them)
 */
static void OnRigidBodyDestroy(Engine::Core::Registry &registry, Engine::EntityId entityId)
{
    try
    {
        auto &core = registry.ctx().get<Engine::Core *>();
        Engine::Entity entity{*core, entityId};

        auto &physicsManager = core->GetResource<Resource::PhysicsManager>();
        if (!physicsManager.IsPhysicsActivated())
        {
            return;
        }

        auto *internalComponent = entity.TryGetComponent<Component::RigidBodyInternal>();
        if (!internalComponent || !internalComponent->IsValid())
        {
            return;
        }

        auto &bodyInterface = physicsManager.GetBodyInterface();
        bodyInterface.RemoveBody(internalComponent->bodyID);
        bodyInterface.DestroyBody(internalComponent->bodyID);

        Log::Debug(fmt::format("Destroyed RigidBody for entity {} with BodyID {}", entity,
                               internalComponent->bodyID.GetIndexAndSequenceNumber()));

        entity.RemoveComponent<Component::RigidBodyInternal>();
    }
    catch (const Exception::RigidBodyError &e)
    {
        Log::Error(fmt::format("RigidBodyError in OnRigidBodyDestroy: {}", e.what()));
    }
}

static void OnRigidBodyInternalDestroy(Engine::Core::Registry &registry, Engine::EntityId entityId)
{
    auto &core = registry.ctx().get<Engine::Core *>();
    Engine::Entity entity{*core, entityId};

    auto internalComponent = entity.GetComponents<Component::RigidBodyInternal>();

    if (!internalComponent.IsValid())
    {
        Log::Warn("RigidBodyInternal has invalid BodyID, skipping body removal from BodyEntityMap");
        return;
    }

    core->GetResource<Resource::BodyEntityMap>().Remove(internalComponent.bodyID);
}

//=============================================================================
// Public System Function
//=============================================================================

void InitRigidBodySystem(Engine::Core &core)
{
    auto &registry = core.GetRegistry();

    registry.ctx().emplace<Engine::Core *>(&core);

    registry.on_construct<Component::RigidBody>().connect<&OnRigidBodyConstruct>();
    registry.on_construct<Component::RigidBodyInternal>().connect<&OnRigidBodyInternalConstruct>();
    registry.on_destroy<Component::RigidBody>().connect<&OnRigidBodyDestroy>();
    registry.on_destroy<Component::RigidBodyInternal>().connect<&OnRigidBodyInternalDestroy>();
}

void ShutdownRigidBodySystem(Engine::Core &core)
{
    auto &registry = core.GetRegistry();

    registry.on_construct<Component::RigidBody>().disconnect<&OnRigidBodyConstruct>();
    registry.on_construct<Component::RigidBodyInternal>().disconnect<&OnRigidBodyInternalConstruct>();
    registry.on_destroy<Component::RigidBody>().disconnect<&OnRigidBodyDestroy>();
    registry.on_destroy<Component::RigidBodyInternal>().disconnect<&OnRigidBodyInternalDestroy>();
}

} // namespace Physics::System
