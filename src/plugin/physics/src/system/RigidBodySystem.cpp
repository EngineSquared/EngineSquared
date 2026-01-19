#include "Physics.pch.hpp"

#include "system/RigidBodySystem.hpp"

#include "Logger.hpp"
#include "component/BoxCollider.hpp"
#include "component/CapsuleCollider.hpp"
#include "component/DefaultCollider.hpp"
#include "component/RigidBody.hpp"
#include "component/RigidBodyInternal.hpp"
#include "component/SphereCollider.hpp"
#include "exception/RigidBodyError.hpp"
#include "resource/PhysicsManager.hpp"
#include "utils/JoltConversions.hpp"
#include <fmt/format.h>

#include "Object.hpp"

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>

namespace Physics::System {

//=============================================================================
// Collider shape creation
//=============================================================================

/**
 * @brief Create a Jolt shape from collider components
 * @return Shared pointer to shape, or nullptr if no collider found
 *
 * @note Priority order when multiple colliders exist:
 * 1. SphereCollider
 * 2. CapsuleCollider
 * 3. BoxCollider
 * 4. DefaultCollider
 *
 * @note If no collider is found, a DefaultCollider will be created automatically.
 */
static JPH::RefConst<JPH::Shape> CreateShapeFromColliders(Engine::Core::Registry &registry, Engine::EntityId entity)
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

    if (auto *defaultCollider = registry.try_get<Component::DefaultCollider>(entity))
    {
        auto *baseShape = new JPH::BoxShape(Utils::ToJoltVec3(defaultCollider->halfExtents));
        if (defaultCollider->offset != glm::vec3{0.0f, 0.0f, 0.0f})
            return new JPH::RotatedTranslatedShape(Utils::ToJoltVec3(defaultCollider->offset), JPH::Quat::sIdentity(),
                                                   baseShape);
        return baseShape;
    }

    return nullptr;
}

/**
 * @brief Get or create a collider shape for the entity
 *
 * If no collider exists, creates a DefaultCollider automatically.
 */
static JPH::RefConst<JPH::Shape> GetOrCreateColliderShape(Engine::Core::Registry &registry, Engine::EntityId entity)
{
    auto shape = CreateShapeFromColliders(registry, entity);

    if (shape != nullptr)
        return shape;

    registry.emplace<Component::DefaultCollider>(entity);
    shape = CreateShapeFromColliders(registry, entity);

    if (shape == nullptr)
    {
        Log::Error("Failed to create default collider shape");
        return nullptr;
    }

    return shape;
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
static void OnRigidBodyConstruct(Engine::Core::Registry &registry, Engine::EntityId entity)
{
    try
    {
        auto &core = registry.ctx().get<Engine::Core *>();

        auto &physicsManager = core->GetResource<Resource::PhysicsManager>();
        if (!physicsManager.IsPhysicsActivated())
        {
            Log::Error("Cannot create RigidBody: Physics system not activated");
            return;
        }

        auto &rigidBody = registry.get<Component::RigidBody>(entity);

        auto *transform = registry.try_get<Object::Component::Transform>(entity);
        if (!transform)
        {
            Log::Warn("RigidBody added to entity without Transform - creating default Transform");
            transform = &registry.emplace<Object::Component::Transform>(entity);
        }

        auto shape = GetOrCreateColliderShape(registry, entity);
        if (!shape)
        {
            Log::Error("Failed to create collider shape for RigidBody");
            return;
        }

        JPH::BodyCreationSettings bodySettings(shape, Utils::ToJoltVec3(transform->GetPosition()),
                                               Utils::ToJoltQuat(transform->GetRotation()), rigidBody.motionType,
                                               rigidBody.objectLayer);

        bodySettings.mUserData = static_cast<uint64_t>(entity);
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

        registry.emplace<Component::RigidBodyInternal>(entity, bodyID);

        Log::Debug(fmt::format("Created RigidBody for entity {} with BodyID {}", static_cast<uint32_t>(entity),
                               bodyID.GetIndexAndSequenceNumber()));
    }
    catch (const Exception::RigidBodyError &e)
    {
        Log::Error(fmt::format("RigidBodyError in OnRigidBodyConstruct: {}", e.what()));
    }
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
static void OnRigidBodyDestroy(Engine::Core::Registry &registry, Engine::EntityId entity)
{
    try
    {
        auto &core = registry.ctx().get<Engine::Core *>();

        auto &physicsManager = core->GetResource<Resource::PhysicsManager>();
        if (!physicsManager.IsPhysicsActivated())
            return;

        auto *internalComponent = registry.try_get<Component::RigidBodyInternal>(entity);
        if (!internalComponent || !internalComponent->IsValid())
            return;

        auto &bodyInterface = physicsManager.GetBodyInterface();
        bodyInterface.RemoveBody(internalComponent->bodyID);
        bodyInterface.DestroyBody(internalComponent->bodyID);

        Log::Debug(fmt::format("Destroyed RigidBody for entity {} with BodyID {}", static_cast<uint32_t>(entity),
                               internalComponent->bodyID.GetIndexAndSequenceNumber()));

        registry.remove<Component::RigidBodyInternal>(entity);
    }
    catch (const Exception::RigidBodyError &e)
    {
        Log::Error(fmt::format("RigidBodyError in OnRigidBodyDestroy: {}", e.what()));
    }
}

//=============================================================================
// Public System Function
//=============================================================================

void InitRigidBodySystem(Engine::Core &core)
{
    auto &registry = core.GetRegistry();

    registry.ctx().emplace<Engine::Core *>(&core);

    registry.on_construct<Component::RigidBody>().connect<&OnRigidBodyConstruct>();
    registry.on_destroy<Component::RigidBody>().connect<&OnRigidBodyDestroy>();
}

} // namespace Physics::System
