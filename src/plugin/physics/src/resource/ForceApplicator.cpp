/**************************************************************************
 * EngineSquared v0.1.1
 *
 * EngineSquared is a software package, part of the Engine² organization.
 *
 * This file is part of the EngineSquared project that is under MIT License.
 * Copyright © 2025-present by @EngineSquared, All rights reserved.
 *
 * EngineSquared is a free software: you can redistribute it and/or modify
 * it under the terms of the MIT License. See the project's LICENSE file for
 * the full license text and details.
 *
 * @file ForceApplicator.cpp
 * @brief Implementation of force and impulse application API
 *
 * @author @EngineSquared
 * @version 0.1.1
 * @date 2025-11-05
 **************************************************************************/

#define GLM_ENABLE_EXPERIMENTAL
#include "ForceApplicator.hpp"

#include "JoltConversions.hpp"
#include "Logger.hpp"
#include "PhysicsManager.hpp"
#include "component/RigidBody.hpp"
#include "component/RigidBodyInternal.hpp"

#include <Jolt/Physics/Body/BodyInterface.h>
#include <fmt/format.h>
#include <glm/gtx/string_cast.hpp>

namespace Physics::Resource {

//============================================================================
// Helper Functions
//============================================================================

namespace {

/**
 * @brief Validate that an entity has a RigidBody and get its internal component
 * @return Pointer to RigidBodyInternal, or nullptr if invalid
 */
inline Component::RigidBodyInternal *GetValidRigidBodyInternal(Engine::Core &core, Engine::Entity entity,
                                                               const char *functionName)
{
    auto &registry = core.GetRegistry();
    entt::entity enttEntity = static_cast<entt::entity>(entity);

    // Check if entity has RigidBody
    if (!registry.all_of<Component::RigidBody>(enttEntity))
    {
        Log::Error(
            fmt::format("{}: Entity {} has no RigidBody component", functionName, entt::to_integral(enttEntity)));
        return nullptr;
    }

    // Check if entity has internal component (should always be present with RigidBody)
    if (!registry.all_of<Component::RigidBodyInternal>(enttEntity))
    {
        Log::Error(fmt::format("{}: Entity {} has RigidBody but no RigidBodyInternal "
                               "(internal error)",
                               functionName, entt::to_integral(enttEntity)));
        return nullptr;
    }

    auto &rigidBody = registry.get<Component::RigidBody>(enttEntity);

    // Warn if trying to apply forces to non-dynamic bodies
    if (rigidBody.motionType != Component::MotionType::Dynamic)
    {
        Log::Warn(fmt::format("{}: Cannot apply force/impulse to non-Dynamic body "
                              "(entity {}, motion type: {})",
                              functionName, entt::to_integral(enttEntity), static_cast<int>(rigidBody.motionType)));
        return nullptr;
    }

    return &registry.get<Component::RigidBodyInternal>(enttEntity);
}

/**
 * @brief Validate that a vector is finite (no NaN or Inf)
 */
inline bool IsFinite(const glm::vec3 &v) { return std::isfinite(v.x) && std::isfinite(v.y) && std::isfinite(v.z); }

} // anonymous namespace

//============================================================================
// CONTINUOUS FORCES
//============================================================================

void AddForce(Engine::Core &core, Engine::Entity entity, const glm::vec3 &force)
{
    // Validate force vector
    if (!IsFinite(force))
    {
        Log::Error(fmt::format("AddForce: Invalid force vector (NaN or Inf): {}", glm::to_string(force)));
        return;
    }

    // Get and validate RigidBody
    auto *internal = GetValidRigidBodyInternal(core, entity, "AddForce");
    if (!internal)
        return;

    // Get BodyInterface and apply force
    auto &physicsManager = core.GetResource<PhysicsManager>();
    JPH::BodyInterface &bodyInterface = physicsManager.GetBodyInterface();

    JPH::Vec3 joltForce = ToJoltVec3(force);
    bodyInterface.AddForce(internal->bodyID, joltForce);
}

void AddForceAtPoint(Engine::Core &core, Engine::Entity entity, const glm::vec3 &force, const glm::vec3 &worldPoint)
{
    // Validate force vector and point
    if (!IsFinite(force))
    {
        Log::Error(fmt::format("AddForceAtPoint: Invalid force vector (NaN or Inf): {}", glm::to_string(force)));
        return;
    }

    if (!IsFinite(worldPoint))
    {
        Log::Error(fmt::format("AddForceAtPoint: Invalid world point (NaN or Inf): {}", glm::to_string(worldPoint)));
        return;
    }

    // Get and validate RigidBody
    auto *internal = GetValidRigidBodyInternal(core, entity, "AddForceAtPoint");
    if (!internal)
        return;

    // Get BodyInterface and apply force at point
    auto &physicsManager = core.GetResource<PhysicsManager>();
    JPH::BodyInterface &bodyInterface = physicsManager.GetBodyInterface();

    JPH::Vec3 joltForce = ToJoltVec3(force);
    JPH::RVec3 joltPoint = ToJoltVec3(worldPoint); // RVec3 for world positions

    bodyInterface.AddForce(internal->bodyID, joltForce, joltPoint);
}

void AddTorque(Engine::Core &core, Engine::Entity entity, const glm::vec3 &torque)
{
    // Validate torque vector
    if (!IsFinite(torque))
    {
        Log::Error(fmt::format("AddTorque: Invalid torque vector (NaN or Inf): {}", glm::to_string(torque)));
        return;
    }

    // Get and validate RigidBody
    auto *internal = GetValidRigidBodyInternal(core, entity, "AddTorque");
    if (!internal)
        return;

    // Get BodyInterface and apply torque
    auto &physicsManager = core.GetResource<PhysicsManager>();
    JPH::BodyInterface &bodyInterface = physicsManager.GetBodyInterface();

    JPH::Vec3 joltTorque = ToJoltVec3(torque);
    bodyInterface.AddTorque(internal->bodyID, joltTorque);
}

//============================================================================
// INSTANTANEOUS IMPULSES
//============================================================================

void AddImpulse(Engine::Core &core, Engine::Entity entity, const glm::vec3 &impulse)
{
    // Validate impulse vector
    if (!IsFinite(impulse))
    {
        Log::Error(fmt::format("AddImpulse: Invalid impulse vector (NaN or Inf): {}", glm::to_string(impulse)));
        return;
    }

    // Get and validate RigidBody
    auto *internal = GetValidRigidBodyInternal(core, entity, "AddImpulse");
    if (!internal)
        return;

    // Get BodyInterface and apply impulse
    auto &physicsManager = core.GetResource<PhysicsManager>();
    JPH::BodyInterface &bodyInterface = physicsManager.GetBodyInterface();

    JPH::Vec3 joltImpulse = ToJoltVec3(impulse);
    bodyInterface.AddImpulse(internal->bodyID, joltImpulse);
}

void AddImpulseAtPoint(Engine::Core &core, Engine::Entity entity, const glm::vec3 &impulse, const glm::vec3 &worldPoint)
{
    // Validate impulse vector and point
    if (!IsFinite(impulse))
    {
        Log::Error(fmt::format("AddImpulseAtPoint: Invalid impulse vector (NaN or Inf): {}", glm::to_string(impulse)));
        return;
    }

    if (!IsFinite(worldPoint))
    {
        Log::Error(fmt::format("AddImpulseAtPoint: Invalid world point (NaN or Inf): {}", glm::to_string(worldPoint)));
        return;
    }

    // Get and validate RigidBody
    auto *internal = GetValidRigidBodyInternal(core, entity, "AddImpulseAtPoint");
    if (!internal)
        return;

    // Get BodyInterface and apply impulse at point
    auto &physicsManager = core.GetResource<PhysicsManager>();
    JPH::BodyInterface &bodyInterface = physicsManager.GetBodyInterface();

    JPH::Vec3 joltImpulse = ToJoltVec3(impulse);
    JPH::RVec3 joltPoint = ToJoltVec3(worldPoint); // RVec3 for world positions

    bodyInterface.AddImpulse(internal->bodyID, joltImpulse, joltPoint);
}

void AddAngularImpulse(Engine::Core &core, Engine::Entity entity, const glm::vec3 &angularImpulse)
{
    // Validate angular impulse vector
    if (!IsFinite(angularImpulse))
    {
        Log::Error(fmt::format("AddAngularImpulse: Invalid angular impulse vector (NaN or Inf): {}",
                               glm::to_string(angularImpulse)));
        return;
    }

    // Get and validate RigidBody
    auto *internal = GetValidRigidBodyInternal(core, entity, "AddAngularImpulse");
    if (!internal)
        return;

    // Get BodyInterface and apply angular impulse
    auto &physicsManager = core.GetResource<PhysicsManager>();
    JPH::BodyInterface &bodyInterface = physicsManager.GetBodyInterface();

    JPH::Vec3 joltAngularImpulse = ToJoltVec3(angularImpulse);
    bodyInterface.AddAngularImpulse(internal->bodyID, joltAngularImpulse);
}

} // namespace Physics::Resource
