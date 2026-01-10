#include "RigidBodyController.hpp"

#include "Logger.hpp"
#include "component/RigidBody.hpp"
#include "component/RigidBodyInternal.hpp"
#include "resource/PhysicsManager.hpp"
#include "utils/JoltConversions.hpp"

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyLockInterface.h>
#include <Jolt/Physics/Body/MotionProperties.h>

#include <fmt/format.h>

namespace Physics::Helper {

static inline JPH::EAllowedDOFs BuildAllowedDOFs(bool lockPosX, bool lockPosY, bool lockPosZ, bool lockRotX,
                                                 bool lockRotY, bool lockRotZ)
{
    JPH::EAllowedDOFs allowed = JPH::EAllowedDOFs::All;
    if (lockPosX)
        allowed &= ~JPH::EAllowedDOFs::TranslationX;
    if (lockPosY)
        allowed &= ~JPH::EAllowedDOFs::TranslationY;
    if (lockPosZ)
        allowed &= ~JPH::EAllowedDOFs::TranslationZ;
    if (lockRotX)
        allowed &= ~JPH::EAllowedDOFs::RotationX;
    if (lockRotY)
        allowed &= ~JPH::EAllowedDOFs::RotationY;
    if (lockRotZ)
        allowed &= ~JPH::EAllowedDOFs::RotationZ;
    return allowed;
}

static inline JPH::MassProperties BuildMassPropertiesFromMotionProperties(const JPH::MotionProperties *mp)
{
    JPH::MassProperties massProps;
    // Reconstruct mass
    float invMass = mp->GetInverseMass();
    if (invMass > 0.0f)
        massProps.mMass = 1.0f / invMass;
    else
        massProps.mMass = 0.0f;

    // Reconstruct inertia from local-space inverse inertia
    JPH::Mat44 invInertia = mp->GetLocalSpaceInverseInertia();
    // Inverse the 3x3 part. If singular, this might produce invalid results; guard via zeroing if needed.
    JPH::Mat44 inertia = invInertia.Inversed3x3();
    massProps.mInertia = inertia;
    return massProps;
}

void SetFriction(Engine::Core &core, Engine::Entity entity, float friction)
{
    auto &registry = core.GetRegistry();
    entt::entity enttEntity = static_cast<entt::entity>(entity);

    if (!registry.all_of<Component::RigidBodyInternal>(enttEntity))
    {
        Log::Error(fmt::format("[RigidBodyController] SetFriction: Entity {} has no RigidBody",
                               entt::to_integral(enttEntity)));
        return;
    }

    auto &rb = registry.get<Component::RigidBody>(enttEntity);
    rb.friction = std::max(0.0f, friction);

    auto &internal = registry.get<Component::RigidBodyInternal>(enttEntity);
    auto &physicsManager = core.GetResource<Resource::PhysicsManager>();
    auto &bodyInterface = physicsManager.GetBodyInterface();

    bodyInterface.SetFriction(internal.bodyID, rb.friction);
}

void SetRestitution(Engine::Core &core, Engine::Entity entity, float restitution)
{
    auto &registry = core.GetRegistry();
    entt::entity enttEntity = static_cast<entt::entity>(entity);

    if (!registry.all_of<Component::RigidBodyInternal>(enttEntity))
    {
        Log::Error(fmt::format("[RigidBodyController] SetRestitution: Entity {} has no RigidBody",
                               entt::to_integral(enttEntity)));
        return;
    }

    restitution = std::clamp(restitution, 0.0f, 1.0f);
    auto &rb = registry.get<Component::RigidBody>(enttEntity);
    rb.restitution = restitution;

    auto &internal = registry.get<Component::RigidBodyInternal>(enttEntity);
    auto &physicsManager = core.GetResource<Resource::PhysicsManager>();
    auto &bodyInterface = physicsManager.GetBodyInterface();

    bodyInterface.SetRestitution(internal.bodyID, rb.restitution);
}

void SetMotionQuality(Engine::Core &core, Engine::Entity entity, Component::MotionQuality motionQuality,
                      bool useLinearCast)
{
    auto &registry = core.GetRegistry();
    entt::entity enttEntity = static_cast<entt::entity>(entity);

    if (!registry.all_of<Component::RigidBodyInternal>(enttEntity))
    {
        Log::Error(fmt::format("[RigidBodyController] SetMotionQuality: Entity {} has no RigidBody",
                               entt::to_integral(enttEntity)));
        return;
    }

    auto &rb = registry.get<Component::RigidBody>(enttEntity);
    rb.motionQuality = motionQuality;
    rb.useMotionQualityLinearCast = useLinearCast;

    auto &internal = registry.get<Component::RigidBodyInternal>(enttEntity);
    auto &physicsManager = core.GetResource<Resource::PhysicsManager>();
    auto &bodyInterface = physicsManager.GetBodyInterface();

    if (useLinearCast)
        bodyInterface.SetMotionQuality(internal.bodyID, JPH::EMotionQuality::LinearCast);
    else
        bodyInterface.SetMotionQuality(internal.bodyID, motionQuality);
}

void SetGravityFactor(Engine::Core &core, Engine::Entity entity, float gravityFactor)
{
    auto &registry = core.GetRegistry();
    entt::entity enttEntity = static_cast<entt::entity>(entity);

    if (!registry.all_of<Component::RigidBodyInternal>(enttEntity))
    {
        Log::Error(fmt::format("[RigidBodyController] SetGravityFactor: Entity {} has no RigidBody",
                               entt::to_integral(enttEntity)));
        return;
    }

    auto &rb = registry.get<Component::RigidBody>(enttEntity);
    rb.gravityFactor = gravityFactor;

    auto &internal = registry.get<Component::RigidBodyInternal>(enttEntity);
    auto &physicsManager = core.GetResource<Resource::PhysicsManager>();
    auto &bodyInterface = physicsManager.GetBodyInterface();

    bodyInterface.SetGravityFactor(internal.bodyID, rb.gravityFactor);
}

void SetEnhancedInternalEdgeRemoval(Engine::Core &core, Engine::Entity entity, bool enabled)
{
    auto &registry = core.GetRegistry();
    entt::entity enttEntity = static_cast<entt::entity>(entity);

    if (!registry.all_of<Component::RigidBodyInternal>(enttEntity))
    {
        Log::Error(fmt::format("[RigidBodyController] SetEnhancedInternalEdgeRemoval: Entity {} has no RigidBody",
                               entt::to_integral(enttEntity)));
        return;
    }

    auto &rb = registry.get<Component::RigidBody>(enttEntity);
    rb.enhancedInternalEdgeRemoval = enabled;

    auto &internal = registry.get<Component::RigidBodyInternal>(enttEntity);
    auto &physicsManager = core.GetResource<Resource::PhysicsManager>();

    // Need to lock the body and call Body::SetEnhancedInternalEdgeRemoval
    auto &bodyLockInterface = physicsManager.GetPhysicsSystem().GetBodyLockInterface();
    JPH::Body *body = bodyLockInterface.TryGetBody(internal.bodyID);
    if (body == nullptr)
    {
        Log::Warn(fmt::format("[RigidBodyController] SetEnhancedInternalEdgeRemoval: Body null for Entity {}",
                              entt::to_integral(enttEntity)));
        return;
    }

    body->SetEnhancedInternalEdgeRemoval(enabled);
}

void SetAxisLocks(Engine::Core &core, Engine::Entity entity, bool lockPositionX, bool lockPositionY, bool lockPositionZ,
                  bool lockRotationX, bool lockRotationY, bool lockRotationZ)
{
    auto &registry = core.GetRegistry();
    entt::entity enttEntity = static_cast<entt::entity>(entity);

    if (!registry.all_of<Component::RigidBodyInternal>(enttEntity))
    {
        Log::Error(fmt::format("[RigidBodyController] SetAxisLocks: Entity {} has no RigidBody",
                               entt::to_integral(enttEntity)));
        return;
    }

    auto &rb = registry.get<Component::RigidBody>(enttEntity);
    rb.lockPositionX = lockPositionX;
    rb.lockPositionY = lockPositionY;
    rb.lockPositionZ = lockPositionZ;
    rb.lockRotationX = lockRotationX;
    rb.lockRotationY = lockRotationY;
    rb.lockRotationZ = lockRotationZ;

    auto &internal = registry.get<Component::RigidBodyInternal>(enttEntity);
    auto &physicsManager = core.GetResource<Resource::PhysicsManager>();

    // Build allowed DOFs mask
    JPH::EAllowedDOFs allowed =
        BuildAllowedDOFs(lockPositionX, lockPositionY, lockPositionZ, lockRotationX, lockRotationY, lockRotationZ);

    // If all DOFs are locked for a non-static body, fallback / warn
    auto &bodyInterface = physicsManager.GetBodyInterface();
    JPH::EMotionType motionType = bodyInterface.GetMotionType(internal.bodyID);
    if (allowed == JPH::EAllowedDOFs::None && motionType != JPH::EMotionType::Static)
    {
        Log::Warn(
            "[RigidBodyController] SetAxisLocks: All axis locked for a non-static body - falling back to All DOFs");
        allowed = JPH::EAllowedDOFs::All;
    }

    // Use a write lock to update MotionProperties::SetMassProperties
    auto &bodyLockInterface = physicsManager.GetPhysicsSystem().GetBodyLockInterface();
    JPH::Body *body = bodyLockInterface.TryGetBody(internal.bodyID);
    if (body == nullptr)
    {
        Log::Warn(
            fmt::format("[RigidBodyController] SetAxisLocks: Body null for Entity {}", entt::to_integral(enttEntity)));
        return;
    }

    JPH::MotionProperties *mp = body->GetMotionProperties();
    if (!mp)
    {
        Log::Warn(fmt::format("[RigidBodyController] SetAxisLocks: MotionProperties null for Entity {}",
                              entt::to_integral(enttEntity)));
        return;
    }

    // Rebuild mass properties from existing motion properties
    JPH::MassProperties massProps = BuildMassPropertiesFromMotionProperties(mp);
    mp->SetMassProperties(allowed, massProps);
}

void SetLinearDamping(Engine::Core &core, Engine::Entity entity, float linearDamping)
{
    auto &registry = core.GetRegistry();
    entt::entity enttEntity = static_cast<entt::entity>(entity);

    if (!registry.all_of<Component::RigidBodyInternal>(enttEntity))
    {
        Log::Error(fmt::format("[RigidBodyController] SetLinearDamping: Entity {} has no RigidBody",
                               entt::to_integral(enttEntity)));
        return;
    }

    auto &rb = registry.get<Component::RigidBody>(enttEntity);
    if (linearDamping < 0.0f)
    {
        Log::Warn("[RigidBodyController] SetLinearDamping: damping should be >= 0");
        linearDamping = 0.0f;
    }
    rb.linearDamping = linearDamping;

    auto &internal = registry.get<Component::RigidBodyInternal>(enttEntity);
    auto &physicsManager = core.GetResource<Resource::PhysicsManager>();

    // Set using BodyInterface
    // BodyInterface doesn't provide SetLinearDamping ID-based wrapper in older Jolt versions, but Get/Set available via
    // MotionProperties. We'll use BodyInterface if available, otherwise write via body pointer.
    auto &bodyLockInterface = physicsManager.GetPhysicsSystem().GetBodyLockInterface();
    JPH::Body *body = bodyLockInterface.TryGetBody(internal.bodyID);
    if (body && body->GetMotionProperties())
        body->GetMotionProperties()->SetLinearDamping(rb.linearDamping);
}

void SetAngularDamping(Engine::Core &core, Engine::Entity entity, float angularDamping)
{
    auto &registry = core.GetRegistry();
    entt::entity enttEntity = static_cast<entt::entity>(entity);

    if (!registry.all_of<Component::RigidBodyInternal>(enttEntity))
    {
        Log::Error(fmt::format("[RigidBodyController] SetAngularDamping: Entity {} has no RigidBody",
                               entt::to_integral(enttEntity)));
        return;
    }

    auto &rb = registry.get<Component::RigidBody>(enttEntity);
    if (angularDamping < 0.0f)
    {
        Log::Warn("[RigidBodyController] SetAngularDamping: damping should be >= 0");
        angularDamping = 0.0f;
    }
    rb.angularDamping = angularDamping;

    auto &internal = registry.get<Component::RigidBodyInternal>(enttEntity);
    auto &physicsManager = core.GetResource<Resource::PhysicsManager>();
    {
        auto &bodyLockInterface = physicsManager.GetPhysicsSystem().GetBodyLockInterface();
        JPH::Body *body = bodyLockInterface.TryGetBody(internal.bodyID);
        if (body && body->GetMotionProperties())
            body->GetMotionProperties()->SetAngularDamping(rb.angularDamping);
    }
}

void SetMass(Engine::Core &core, Engine::Entity entity, float mass)
{
    auto &registry = core.GetRegistry();
    entt::entity enttEntity = static_cast<entt::entity>(entity);

    if (!registry.all_of<Component::RigidBodyInternal>(enttEntity))
    {
        Log::Error(
            fmt::format("[RigidBodyController] SetMass: Entity {} has no RigidBody", entt::to_integral(enttEntity)));
        return;
    }

    auto &rb = registry.get<Component::RigidBody>(enttEntity);
    if (mass < 0.0f)
    {
        Log::Warn("[RigidBodyController] SetMass: mass should be >= 0");
        mass = 0.0f;
    }
    rb.mass = mass;

    auto &internal = registry.get<Component::RigidBodyInternal>(enttEntity);
    auto &physicsManager = core.GetResource<Resource::PhysicsManager>();

    // Update mass via MotionProperties on the body pointer
    auto &bodyLockInterface = physicsManager.GetPhysicsSystem().GetBodyLockInterface();
    JPH::Body *body = bodyLockInterface.TryGetBody(internal.bodyID);
    if (body == nullptr)
    {
        Log::Warn(fmt::format("[RigidBodyController] SetMass: Body null for Entity {}", entt::to_integral(enttEntity)));
        return;
    }

    JPH::MotionProperties *mp = body->GetMotionProperties();
    if (!mp)
    {
        Log::Warn(fmt::format("[RigidBodyController] SetMass: MotionProperties null for Entity {}",
                              entt::to_integral(enttEntity)));
        return;
    }

    // Scale mass while keeping relative inertia via ScaleToMass
    if (mass > 0.0f)
    {
        mp->ScaleToMass(mass);
    }
}

void SetMotionType(Engine::Core &core, Engine::Entity entity, JPH::EMotionType motionType, JPH::EActivation activation)
{
    auto &registry = core.GetRegistry();
    entt::entity enttEntity = static_cast<entt::entity>(entity);

    if (!registry.all_of<Component::RigidBodyInternal>(enttEntity))
    {
        Log::Error(fmt::format("[RigidBodyController] SetMotionType: Entity {} has no RigidBody",
                               entt::to_integral(enttEntity)));
        return;
    }

    auto &rb = registry.get<Component::RigidBody>(enttEntity);
    rb.motionType = static_cast<Component::MotionType>(motionType);

    auto &internal = registry.get<Component::RigidBodyInternal>(enttEntity);
    auto &physicsManager = core.GetResource<Resource::PhysicsManager>();
    physicsManager.GetBodyInterface().SetMotionType(internal.bodyID, motionType, activation);
}

void SetAllowSleeping(Engine::Core &core, Engine::Entity entity, bool allowSleeping)
{
    auto &registry = core.GetRegistry();
    entt::entity enttEntity = static_cast<entt::entity>(entity);

    if (!registry.all_of<Component::RigidBodyInternal>(enttEntity))
    {
        Log::Error(fmt::format("[RigidBodyController] SetAllowSleeping: Entity {} has no RigidBody",
                               entt::to_integral(enttEntity)));
        return;
    }

    auto &rb = registry.get<Component::RigidBody>(enttEntity);
    rb.allowSleeping = allowSleeping;

    auto &internal = registry.get<Component::RigidBodyInternal>(enttEntity);
    auto &physicsManager = core.GetResource<Resource::PhysicsManager>();

    auto &bodyLockInterface = physicsManager.GetPhysicsSystem().GetBodyLockInterface();
    JPH::Body *body = bodyLockInterface.TryGetBody(internal.bodyID);
    if (body)
        body->SetAllowSleeping(allowSleeping);
}

} // namespace Physics::Helper
