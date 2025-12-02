#include "VelocityController.hpp"

#include "component/RigidBody.hpp"
#include "component/RigidBodyInternal.hpp"
#include "resource/PhysicsManager.hpp"
#include "utils/JoltConversions.hpp"
#include "Logger.hpp"
#include <fmt/format.h>

#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/compatibility.hpp>

namespace Physics::Helper {

// ============================================================================
// LINEAR VELOCITY - GETTERS
// ============================================================================

glm::vec3 GetLinearVelocity(Engine::Core& core, Engine::Entity entity)
{
    auto& registry = core.GetRegistry();
    entt::entity enttEntity = static_cast<entt::entity>(entity);

    // Validate entity has RigidBody
    if (!registry.all_of<Component::RigidBodyInternal>(enttEntity)) {
        Log::Error(
            fmt::format("[VelocityController] GetLinearVelocity: Entity {} has no RigidBody",
            entt::to_integral(enttEntity)));
        return glm::vec3(0.0f);
    }

    auto& internal = registry.get<Component::RigidBodyInternal>(enttEntity);
    auto& physicsManager = core.GetResource<Resource::PhysicsManager>();

    // Get velocity from Jolt
    JPH::Vec3 velocity =
        physicsManager.GetBodyInterface().GetLinearVelocity(internal.bodyID);

    return Utils::FromJoltVec3(velocity);
}

// ============================================================================
// LINEAR VELOCITY - SETTERS
// ============================================================================

void SetLinearVelocity(Engine::Core& core, Engine::Entity entity,
                       const glm::vec3& velocity)
{
    auto& registry = core.GetRegistry();
    entt::entity enttEntity = static_cast<entt::entity>(entity);

    // Validate entity has RigidBody
    if (!registry.all_of<Component::RigidBodyInternal>(enttEntity)) {
        Log::Error(
            fmt::format("[VelocityController] SetLinearVelocity: Entity {} has no RigidBody",
            entt::to_integral(enttEntity)));
        return;
    }

    // Validate velocity values (no NaN/Inf)
    if (!glm::all(glm::isfinite(velocity))) {
        Log::Error(
            fmt::format("[VelocityController] SetLinearVelocity: Invalid velocity (NaN or Inf)"));
        return;
    }

    // Check if body is static
    auto& rb = registry.get<Component::RigidBody>(enttEntity);
    if (rb.motionType == Component::MotionType::Static) {
        Log::Warn(
            fmt::format("[VelocityController] SetLinearVelocity: Cannot set velocity on static "
            "body (Entity {})",
            entt::to_integral(enttEntity)));
        return;
    }

    auto& internal = registry.get<Component::RigidBodyInternal>(enttEntity);
    auto& physicsManager = core.GetResource<Resource::PhysicsManager>();
    auto& bodyInterface = physicsManager.GetBodyInterface();

    // Convert and set velocity
    JPH::Vec3 joltVelocity = Utils::ToJoltVec3(velocity);
    bodyInterface.SetLinearVelocity(internal.bodyID, joltVelocity);

    // Activate body if sleeping
    if (!bodyInterface.IsActive(internal.bodyID)) {
        bodyInterface.ActivateBody(internal.bodyID);
    }
}

void AddLinearVelocity(Engine::Core& core, Engine::Entity entity,
                       const glm::vec3& deltaVelocity)
{
    // Get current velocity
    glm::vec3 currentVelocity = GetLinearVelocity(core, entity);

    // Add delta and set
    SetLinearVelocity(core, entity, currentVelocity + deltaVelocity);
}

// ============================================================================
// ANGULAR VELOCITY - GETTERS
// ============================================================================

glm::vec3 GetAngularVelocity(Engine::Core& core, Engine::Entity entity)
{
    auto& registry = core.GetRegistry();
    entt::entity enttEntity = static_cast<entt::entity>(entity);

    // Validate entity has RigidBody
    if (!registry.all_of<Component::RigidBodyInternal>(enttEntity)) {
        Log::Error(
            fmt::format("[VelocityController] GetAngularVelocity: Entity {} has no RigidBody",
            entt::to_integral(enttEntity)));
        return glm::vec3(0.0f);
    }

    auto& internal = registry.get<Component::RigidBodyInternal>(enttEntity);
    auto& physicsManager = core.GetResource<Resource::PhysicsManager>();

    // Get angular velocity from Jolt
    JPH::Vec3 angularVelocity =
        physicsManager.GetBodyInterface().GetAngularVelocity(internal.bodyID);

    return Utils::FromJoltVec3(angularVelocity);
}

// ============================================================================
// ANGULAR VELOCITY - SETTERS
// ============================================================================

void SetAngularVelocity(Engine::Core& core, Engine::Entity entity,
                        const glm::vec3& angularVelocity)
{
    auto& registry = core.GetRegistry();
    entt::entity enttEntity = static_cast<entt::entity>(entity);

    // Validate entity has RigidBody
    if (!registry.all_of<Component::RigidBodyInternal>(enttEntity)) {
        Log::Error(
            fmt::format("[VelocityController] SetAngularVelocity: Entity {} has no RigidBody",
            entt::to_integral(enttEntity)));
        return;
    }

    // Validate velocity values (no NaN/Inf)
    if (!glm::all(glm::isfinite(angularVelocity))) {
        Log::Error(
            fmt::format("[VelocityController] SetAngularVelocity: Invalid angular velocity (NaN "
            "or Inf)"));
        return;
    }

    // Check if body is static
    auto& rb = registry.get<Component::RigidBody>(enttEntity);
    if (rb.motionType == Component::MotionType::Static) {
        Log::Warn(
            fmt::format("[VelocityController] SetAngularVelocity: Cannot set angular velocity on "
            "static body (Entity {})",
            entt::to_integral(enttEntity)));
        return;
    }

    auto& internal = registry.get<Component::RigidBodyInternal>(enttEntity);
    auto& physicsManager = core.GetResource<Resource::PhysicsManager>();
    auto& bodyInterface = physicsManager.GetBodyInterface();

    // Convert and set angular velocity
    JPH::Vec3 joltAngularVelocity = Utils::ToJoltVec3(angularVelocity);
    bodyInterface.SetAngularVelocity(internal.bodyID, joltAngularVelocity);

    // Activate body if sleeping
    if (!bodyInterface.IsActive(internal.bodyID)) {
        bodyInterface.ActivateBody(internal.bodyID);
    }
}

void AddAngularVelocity(Engine::Core& core, Engine::Entity entity,
                        const glm::vec3& deltaAngularVelocity)
{
    // Get current angular velocity
    glm::vec3 currentAngularVelocity = GetAngularVelocity(core, entity);

    // Add delta and set
    SetAngularVelocity(core, entity, currentAngularVelocity + deltaAngularVelocity);
}

}  // namespace Physics::Helper
