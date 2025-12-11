#include "Physics.pch.hpp"

#include "system/ConstraintSystem.hpp"

#include "Logger.hpp"
#include "component/ConstraintInternal.hpp"
#include "component/ConstraintSettings.hpp"
#include "component/DistanceConstraint.hpp"
#include "component/FixedConstraint.hpp"
#include "component/PointConstraint.hpp"
#include "component/RigidBodyInternal.hpp"
#include "exception/ConstraintError.hpp"
#include "resource/PhysicsManager.hpp"
#include "utils/JoltConversions.hpp"
#include <fmt/format.h>

// Jolt constraint headers
#include <Jolt/Physics/Body/BodyLockMulti.h>
#include <Jolt/Physics/Constraints/DistanceConstraint.h>
#include <Jolt/Physics/Constraints/FixedConstraint.h>
#include <Jolt/Physics/Constraints/PointConstraint.h>
#include <Jolt/Physics/PhysicsSystem.h>

namespace Physics::System {

//=============================================================================
// Helper functions
//=============================================================================

/**
 * @brief Get Jolt body from entity, or nullptr if invalid
 */
static JPH::Body *GetBodyFromEntity(entt::registry &registry, Engine::Core &core, Engine::Entity entity)
{
    if (!entity.IsValid())
        return nullptr;

    auto *internalComponent = registry.try_get<Component::RigidBodyInternal>(static_cast<entt::entity>(entity));
    if (!internalComponent || !internalComponent->IsValid())
        return nullptr;

    auto &physicsManager = core.GetResource<Resource::PhysicsManager>();
    auto &bodyInterface = physicsManager.GetBodyInterface();

    // Use body lock to safely access the body
    JPH::BodyLockWrite lock(physicsManager.GetPhysicsSystem().GetBodyLockInterface(), internalComponent->bodyID);
    if (!lock.Succeeded())
        return nullptr;

    return &lock.GetBody();
}

/**
 * @brief Configure spring settings for soft constraints
 */
static void ConfigureSpringSettings(JPH::SpringSettings &springSettings, const Component::ConstraintSettings &settings)
{
    if (!settings.IsRigid())
    {
        // Use stiffness and damping mode for soft constraints
        springSettings.mMode = JPH::ESpringMode::StiffnessAndDamping;
        springSettings.mStiffness = settings.stiffness * 10000.0f; // Scale to reasonable Jolt values
        springSettings.mDamping = settings.damping * 100.0f;
    }
}

//=============================================================================
// Fixed Constraint Handler
//=============================================================================

static void OnFixedConstraintConstruct(entt::registry &registry, entt::entity entity)
{
    try
    {
        auto &core = *registry.ctx().get<Engine::Core *>();

        auto &physicsManager = core.GetResource<Resource::PhysicsManager>();
        if (!physicsManager.IsPhysicsActivated())
        {
            Log::Error("Cannot create FixedConstraint: Physics system not activated");
            return;
        }

        auto &constraint = registry.get<Component::FixedConstraint>(entity);

        // Validate constraint
        if (!constraint.bodyA.IsValid())
        {
            Log::Error("FixedConstraint: bodyA is invalid");
            return;
        }

        if (constraint.bodyA == constraint.bodyB)
        {
            Log::Error("FixedConstraint: Cannot constrain body to itself");
            return;
        }

        // Get body A
        auto *internalA = registry.try_get<Component::RigidBodyInternal>(static_cast<entt::entity>(constraint.bodyA));
        if (!internalA || !internalA->IsValid())
        {
            Log::Error("FixedConstraint: bodyA has no valid RigidBodyInternal");
            return;
        }

        // Configure Jolt settings
        JPH::FixedConstraintSettings joltSettings;
        joltSettings.mSpace = JPH::EConstraintSpace::LocalToBodyCOM;
        joltSettings.mAutoDetectPoint = false;
        joltSettings.mPoint1 = Utils::ToJoltRVec3(constraint.localPointA);
        joltSettings.mPoint2 = Utils::ToJoltRVec3(constraint.localPointB);

        // Create constraint
        auto &physicsSystem = physicsManager.GetPhysicsSystem();
        JPH::Constraint *joltConstraint = nullptr;

        if (constraint.IsWorldConstraint())
        {
            // Body to world constraint - single body lock
            JPH::BodyLockWrite lockA(physicsSystem.GetBodyLockInterface(), internalA->bodyID);
            if (!lockA.Succeeded())
            {
                Log::Error("FixedConstraint: Failed to lock bodyA");
                return;
            }

            // For world constraints, use world space
            joltSettings.mSpace = JPH::EConstraintSpace::WorldSpace;
            joltSettings.mPoint2 = Utils::ToJoltRVec3(constraint.localPointB); // World position

            joltConstraint = joltSettings.Create(lockA.GetBody(), JPH::Body::sFixedToWorld);
        }
        else
        {
            // Body to body constraint - use multi-body lock to avoid deadlock
            auto *internalB =
                registry.try_get<Component::RigidBodyInternal>(static_cast<entt::entity>(constraint.bodyB));
            if (!internalB || !internalB->IsValid())
            {
                Log::Error("FixedConstraint: bodyB has no valid RigidBodyInternal");
                return;
            }

            // Use BodyLockMultiWrite to safely lock both bodies at once
            JPH::BodyID bodyIDs[2] = {internalA->bodyID, internalB->bodyID};
            JPH::BodyLockMultiWrite lock(physicsSystem.GetBodyLockInterface(), bodyIDs, 2);

            JPH::Body *bodyA = lock.GetBody(0);
            JPH::Body *bodyB = lock.GetBody(1);

            if (!bodyA || !bodyB)
            {
                Log::Error("FixedConstraint: Failed to lock bodies");
                return;
            }

            joltConstraint = joltSettings.Create(*bodyA, *bodyB);
        }

        if (!joltConstraint)
        {
            Log::Error("FixedConstraint: Failed to create Jolt constraint");
            return;
        }

        // Add to physics system
        physicsSystem.AddConstraint(joltConstraint);

        // Store internal component
        registry.emplace<Component::ConstraintInternal>(entity, joltConstraint, Component::ConstraintType::Fixed,
                                                        constraint.settings.breakForce,
                                                        constraint.settings.breakTorque);

        Log::Debug(fmt::format("Created FixedConstraint for entity {}", static_cast<uint32_t>(entity)));
    }
    catch (const std::exception &e)
    {
        Log::Error(fmt::format("FixedConstraint error: {}", e.what()));
    }
}

static void OnFixedConstraintDestroy(entt::registry &registry, entt::entity entity)
{
    try
    {
        auto &core = *registry.ctx().get<Engine::Core *>();

        auto &physicsManager = core.GetResource<Resource::PhysicsManager>();
        if (!physicsManager.IsPhysicsActivated())
            return;

        auto *internal = registry.try_get<Component::ConstraintInternal>(entity);
        if (!internal || !internal->IsValid())
            return;

        physicsManager.GetPhysicsSystem().RemoveConstraint(internal->constraint);

        Log::Debug(fmt::format("Destroyed FixedConstraint for entity {}", static_cast<uint32_t>(entity)));

        registry.remove<Component::ConstraintInternal>(entity);
    }
    catch (const std::exception &e)
    {
        Log::Error(fmt::format("FixedConstraint destroy error: {}", e.what()));
    }
}

//=============================================================================
// Distance Constraint Handler
//=============================================================================

static void OnDistanceConstraintConstruct(entt::registry &registry, entt::entity entity)
{
    try
    {
        auto &core = *registry.ctx().get<Engine::Core *>();

        auto &physicsManager = core.GetResource<Resource::PhysicsManager>();
        if (!physicsManager.IsPhysicsActivated())
        {
            Log::Error("Cannot create DistanceConstraint: Physics system not activated");
            return;
        }

        auto &constraint = registry.get<Component::DistanceConstraint>(entity);

        // Validate constraint
        if (!constraint.bodyA.IsValid())
        {
            Log::Error("DistanceConstraint: bodyA is invalid");
            return;
        }

        if (constraint.bodyA == constraint.bodyB)
        {
            Log::Error("DistanceConstraint: Cannot constrain body to itself");
            return;
        }

        if (constraint.maxDistance >= 0 && constraint.minDistance >= 0 &&
            constraint.maxDistance < constraint.minDistance)
        {
            Log::Error("DistanceConstraint: maxDistance < minDistance");
            return;
        }

        // Get body A
        auto *internalA = registry.try_get<Component::RigidBodyInternal>(static_cast<entt::entity>(constraint.bodyA));
        if (!internalA || !internalA->IsValid())
        {
            Log::Error("DistanceConstraint: bodyA has no valid RigidBodyInternal");
            return;
        }

        // Configure Jolt settings
        JPH::DistanceConstraintSettings joltSettings;
        joltSettings.mSpace = JPH::EConstraintSpace::LocalToBodyCOM;
        joltSettings.mPoint1 = Utils::ToJoltRVec3(constraint.localPointA);
        joltSettings.mPoint2 = Utils::ToJoltRVec3(constraint.localPointB);
        joltSettings.mMinDistance = constraint.minDistance;
        joltSettings.mMaxDistance = constraint.maxDistance;

        // Configure soft limits if not rigid
        ConfigureSpringSettings(joltSettings.mLimitsSpringSettings, constraint.settings);

        // Create constraint
        auto &physicsSystem = physicsManager.GetPhysicsSystem();
        JPH::Constraint *joltConstraint = nullptr;

        if (constraint.IsWorldConstraint())
        {
            // Body to world constraint - single body lock
            JPH::BodyLockWrite lockA(physicsSystem.GetBodyLockInterface(), internalA->bodyID);
            if (!lockA.Succeeded())
            {
                Log::Error("DistanceConstraint: Failed to lock bodyA");
                return;
            }

            joltSettings.mSpace = JPH::EConstraintSpace::WorldSpace;
            joltSettings.mPoint2 = Utils::ToJoltRVec3(constraint.localPointB); // World position

            joltConstraint = joltSettings.Create(lockA.GetBody(), JPH::Body::sFixedToWorld);
        }
        else
        {
            // Body to body constraint - use multi-body lock to avoid deadlock
            auto *internalB =
                registry.try_get<Component::RigidBodyInternal>(static_cast<entt::entity>(constraint.bodyB));
            if (!internalB || !internalB->IsValid())
            {
                Log::Error("DistanceConstraint: bodyB has no valid RigidBodyInternal");
                return;
            }

            // Use BodyLockMultiWrite to safely lock both bodies at once
            JPH::BodyID bodyIDs[2] = {internalA->bodyID, internalB->bodyID};
            JPH::BodyLockMultiWrite lock(physicsSystem.GetBodyLockInterface(), bodyIDs, 2);

            JPH::Body *bodyA = lock.GetBody(0);
            JPH::Body *bodyB = lock.GetBody(1);

            if (!bodyA || !bodyB)
            {
                Log::Error("DistanceConstraint: Failed to lock bodies");
                return;
            }

            joltConstraint = joltSettings.Create(*bodyA, *bodyB);
        }

        if (!joltConstraint)
        {
            Log::Error("DistanceConstraint: Failed to create Jolt constraint");
            return;
        }

        // Add to physics system
        physicsSystem.AddConstraint(joltConstraint);

        // Store internal component
        registry.emplace<Component::ConstraintInternal>(entity, joltConstraint, Component::ConstraintType::Distance,
                                                        constraint.settings.breakForce,
                                                        constraint.settings.breakTorque);

        Log::Debug(fmt::format("Created DistanceConstraint for entity {} (min: {}, max: {})",
                               static_cast<uint32_t>(entity), constraint.minDistance, constraint.maxDistance));
    }
    catch (const std::exception &e)
    {
        Log::Error(fmt::format("DistanceConstraint error: {}", e.what()));
    }
}

static void OnDistanceConstraintDestroy(entt::registry &registry, entt::entity entity)
{
    try
    {
        auto &core = *registry.ctx().get<Engine::Core *>();

        auto &physicsManager = core.GetResource<Resource::PhysicsManager>();
        if (!physicsManager.IsPhysicsActivated())
            return;

        auto *internal = registry.try_get<Component::ConstraintInternal>(entity);
        if (!internal || !internal->IsValid())
            return;

        physicsManager.GetPhysicsSystem().RemoveConstraint(internal->constraint);

        Log::Debug(fmt::format("Destroyed DistanceConstraint for entity {}", static_cast<uint32_t>(entity)));

        registry.remove<Component::ConstraintInternal>(entity);
    }
    catch (const std::exception &e)
    {
        Log::Error(fmt::format("DistanceConstraint destroy error: {}", e.what()));
    }
}

//=============================================================================
// Point Constraint Handler
//=============================================================================

static void OnPointConstraintConstruct(entt::registry &registry, entt::entity entity)
{
    try
    {
        auto &core = *registry.ctx().get<Engine::Core *>();

        auto &physicsManager = core.GetResource<Resource::PhysicsManager>();
        if (!physicsManager.IsPhysicsActivated())
        {
            Log::Error("Cannot create PointConstraint: Physics system not activated");
            return;
        }

        auto &constraint = registry.get<Component::PointConstraint>(entity);

        // Validate constraint
        if (!constraint.bodyA.IsValid())
        {
            Log::Error("PointConstraint: bodyA is invalid");
            return;
        }

        if (constraint.bodyA == constraint.bodyB)
        {
            Log::Error("PointConstraint: Cannot constrain body to itself");
            return;
        }

        // Get body A
        auto *internalA = registry.try_get<Component::RigidBodyInternal>(static_cast<entt::entity>(constraint.bodyA));
        if (!internalA || !internalA->IsValid())
        {
            Log::Error("PointConstraint: bodyA has no valid RigidBodyInternal");
            return;
        }

        // Configure Jolt settings
        JPH::PointConstraintSettings joltSettings;
        joltSettings.mSpace = JPH::EConstraintSpace::LocalToBodyCOM;
        joltSettings.mPoint1 = Utils::ToJoltRVec3(constraint.localPointA);
        joltSettings.mPoint2 = Utils::ToJoltRVec3(constraint.localPointB);

        // Create constraint
        auto &physicsSystem = physicsManager.GetPhysicsSystem();
        JPH::Constraint *joltConstraint = nullptr;

        if (constraint.IsWorldConstraint())
        {
            // Body to world constraint - single body lock
            JPH::BodyLockWrite lockA(physicsSystem.GetBodyLockInterface(), internalA->bodyID);
            if (!lockA.Succeeded())
            {
                Log::Error("PointConstraint: Failed to lock bodyA");
                return;
            }

            joltSettings.mSpace = JPH::EConstraintSpace::WorldSpace;
            joltSettings.mPoint2 = Utils::ToJoltRVec3(constraint.localPointB); // World position

            joltConstraint = joltSettings.Create(lockA.GetBody(), JPH::Body::sFixedToWorld);
        }
        else
        {
            // Body to body constraint - use multi-body lock to avoid deadlock
            auto *internalB =
                registry.try_get<Component::RigidBodyInternal>(static_cast<entt::entity>(constraint.bodyB));
            if (!internalB || !internalB->IsValid())
            {
                Log::Error("PointConstraint: bodyB has no valid RigidBodyInternal");
                return;
            }

            // Use BodyLockMultiWrite to safely lock both bodies at once
            JPH::BodyID bodyIDs[2] = {internalA->bodyID, internalB->bodyID};
            JPH::BodyLockMultiWrite lock(physicsSystem.GetBodyLockInterface(), bodyIDs, 2);

            JPH::Body *bodyA = lock.GetBody(0);
            JPH::Body *bodyB = lock.GetBody(1);

            if (!bodyA || !bodyB)
            {
                Log::Error("PointConstraint: Failed to lock bodies");
                return;
            }

            joltConstraint = joltSettings.Create(*bodyA, *bodyB);
        }

        if (!joltConstraint)
        {
            Log::Error("PointConstraint: Failed to create Jolt constraint");
            return;
        }

        // Add to physics system
        physicsSystem.AddConstraint(joltConstraint);

        // Store internal component
        registry.emplace<Component::ConstraintInternal>(entity, joltConstraint, Component::ConstraintType::Point,
                                                        constraint.settings.breakForce,
                                                        constraint.settings.breakTorque);

        Log::Debug(fmt::format("Created PointConstraint for entity {}", static_cast<uint32_t>(entity)));
    }
    catch (const std::exception &e)
    {
        Log::Error(fmt::format("PointConstraint error: {}", e.what()));
    }
}

static void OnPointConstraintDestroy(entt::registry &registry, entt::entity entity)
{
    try
    {
        auto &core = *registry.ctx().get<Engine::Core *>();

        auto &physicsManager = core.GetResource<Resource::PhysicsManager>();
        if (!physicsManager.IsPhysicsActivated())
            return;

        auto *internal = registry.try_get<Component::ConstraintInternal>(entity);
        if (!internal || !internal->IsValid())
            return;

        physicsManager.GetPhysicsSystem().RemoveConstraint(internal->constraint);

        Log::Debug(fmt::format("Destroyed PointConstraint for entity {}", static_cast<uint32_t>(entity)));

        registry.remove<Component::ConstraintInternal>(entity);
    }
    catch (const std::exception &e)
    {
        Log::Error(fmt::format("PointConstraint destroy error: {}", e.what()));
    }
}

//=============================================================================
// Public System Function
//=============================================================================

void InitConstraintSystem(Engine::Core &core)
{
    auto &registry = core.GetRegistry();

    // Ensure core pointer is in context (should already be set by RigidBodySystem)
    if (!registry.ctx().contains<Engine::Core *>())
    {
        registry.ctx().emplace<Engine::Core *>(&core);
    }

    // Register FixedConstraint hooks
    registry.on_construct<Component::FixedConstraint>().connect<&OnFixedConstraintConstruct>();
    registry.on_destroy<Component::FixedConstraint>().connect<&OnFixedConstraintDestroy>();

    // Register DistanceConstraint hooks
    registry.on_construct<Component::DistanceConstraint>().connect<&OnDistanceConstraintConstruct>();
    registry.on_destroy<Component::DistanceConstraint>().connect<&OnDistanceConstraintDestroy>();

    // Register PointConstraint hooks
    registry.on_construct<Component::PointConstraint>().connect<&OnPointConstraintConstruct>();
    registry.on_destroy<Component::PointConstraint>().connect<&OnPointConstraintDestroy>();

    Log::Info("ConstraintSystem initialized");
}

} // namespace Physics::System
