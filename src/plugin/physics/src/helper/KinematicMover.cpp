#include "KinematicMover.hpp"

#include "Logger.hpp"
#include "Object.hpp"
#include "component/RigidBody.hpp"
#include "component/RigidBodyInternal.hpp"
#include "resource/PhysicsManager.hpp"
#include "utils/JoltConversions.hpp"
#include <fmt/format.h>

#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/compatibility.hpp>

namespace Physics::Helper {

// ============================================================================
// KINEMATIC MOVEMENT
// ============================================================================

void MoveKinematic(Engine::Core &core, Engine::Entity entity, const glm::vec3 &targetPosition,
                   const glm::quat &targetRotation, float deltaTime)
{
    auto &registry = core.GetRegistry();
    entt::entity enttEntity = static_cast<entt::entity>(entity);

    // Validate entity has RigidBody
    if (!registry.all_of<Component::RigidBody, Component::RigidBodyInternal>(enttEntity))
    {
        Log::Error(
            fmt::format("[KinematicMover] MoveKinematic: Entity {} has no RigidBody", entt::to_integral(enttEntity)));
        return;
    }

    // Validate targetPosition (must be finite)
    if (!glm::all(glm::isfinite(targetPosition)))
    {
        Log::Error(fmt::format("MoveKinematic: targetPosition contains NaN or Inf values ({}, {}, {})",
                               targetPosition.x, targetPosition.y, targetPosition.z));
        return;
    }

    // Validate targetRotation (must be finite)
    if (!Utils::IsFinite(targetRotation))
    {
        Log::Error(fmt::format("MoveKinematic: targetRotation contains NaN or Inf values"));
        return;
    }

    // Validate deltaTime
    if (deltaTime <= 0.0f)
    {
        Log::Error(fmt::format("[KinematicMover] MoveKinematic: deltaTime must be positive (got {})", deltaTime));
        return;
    }

    // Check if body is actually kinematic
    auto &rb = registry.get<Component::RigidBody>(enttEntity);
    if (rb.motionType != Component::MotionType::Kinematic)
    {
        Log::Warn(fmt::format("[KinematicMover] MoveKinematic: Body is not kinematic (Entity {}, MotionType: {})",
                              entt::to_integral(enttEntity), static_cast<int>(rb.motionType)));
        return;
    }

    auto &internal = registry.get<Component::RigidBodyInternal>(enttEntity);
    auto &physicsManager = core.GetResource<Resource::PhysicsManager>();

    // Convert to Jolt types
    JPH::RVec3 joltTarget = Utils::ToJoltVec3(targetPosition);
    JPH::Quat joltRotation = Utils::ToJoltQuat(targetRotation);

    // Move kinematic body
    physicsManager.GetBodyInterface().MoveKinematic(internal.bodyID, joltTarget, joltRotation, deltaTime);
}

void SetKinematicVelocity(Engine::Core &core, Engine::Entity entity, const glm::vec3 &velocity, float deltaTime)
{
    auto &registry = core.GetRegistry();
    entt::entity enttEntity = static_cast<entt::entity>(entity);

    // Validate entity has Transform and RigidBody
    if (!registry.all_of<Object::Component::Transform, Component::RigidBody>(enttEntity))
    {
        Log::Error(fmt::format("[KinematicMover] SetKinematicVelocity: Entity {} missing Transform or RigidBody",
                               entt::to_integral(enttEntity)));
        return;
    }

    // Validate velocity
    if (!glm::all(glm::isfinite(velocity)))
    {
        Log::Error(fmt::format("[KinematicMover] SetKinematicVelocity: Invalid velocity (NaN or Inf)"));
        return;
    }

    // Validate deltaTime
    if (deltaTime <= 0.0f)
    {
        Log::Error(
            fmt::format("[KinematicMover] SetKinematicVelocity: deltaTime must be positive (got {})", deltaTime));
        return;
    }

    // Get current position and rotation
    auto &transform = registry.get<Object::Component::Transform>(enttEntity);
    glm::vec3 currentPosition = transform.GetPosition();
    glm::quat currentRotation = transform.GetRotation();

    // Calculate target position
    glm::vec3 targetPosition = currentPosition + velocity * deltaTime;

    // Move to target (rotation unchanged)
    MoveKinematic(core, entity, targetPosition, currentRotation, deltaTime);
}

// ============================================================================
// KINEMATIC STATE QUERY
// ============================================================================

KinematicTarget GetKinematicTarget(Engine::Core &core, Engine::Entity entity)
{
    auto &registry = core.GetRegistry();
    entt::entity enttEntity = static_cast<entt::entity>(entity);

    KinematicTarget result;
    result.hasTarget = false;

    // Validate entity has RigidBody
    if (!registry.all_of<Component::RigidBody, Component::RigidBodyInternal>(enttEntity))
        return result;

    // Check if body is kinematic
    auto &rb = registry.get<Component::RigidBody>(enttEntity);
    if (rb.motionType != Component::MotionType::Kinematic)
        return result;

    auto &internal = registry.get<Component::RigidBodyInternal>(enttEntity);
    auto &physicsManager = core.GetResource<Resource::PhysicsManager>();

    // Get current position and rotation from Jolt
    // (Kinematic bodies don't store explicit "targets", so we return current state)
    JPH::RVec3 joltPosition = physicsManager.GetBodyInterface().GetPosition(internal.bodyID);
    JPH::Quat joltRotation = physicsManager.GetBodyInterface().GetRotation(internal.bodyID);

    result.position = Utils::FromJoltRVec3(joltPosition);
    result.rotation = Utils::FromJoltQuat(joltRotation);
    result.hasTarget = true;

    return result;
}

} // namespace Physics::Helper
