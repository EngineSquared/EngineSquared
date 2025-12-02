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
 * @file VelocityController.hpp
 * @brief API for direct velocity control
 *
 * This API provides a clean interface for direct velocity control of rigid bodies.
 * It handles all the necessary conversions and error checking.
 *
 * @author @EngineSquared
 * @version 0.1.1
 * @date 2025-11-05
 **************************************************************************/

#pragma once

#include "core/Core.hpp"
#include "entity/Entity.hpp"

#include <glm/glm.hpp>

namespace Physics::Helper {

// ============================================================================
// LINEAR VELOCITY CONTROL
// ============================================================================

/**
 * @brief Set the linear velocity of a rigid body (world space)
 *
 * Sets the instantaneous linear velocity of the body. This provides direct
 * control over movement, bypassing forces and acceleration.
 *
 * Use cases:
 * - Character controllers (direct movement)
 * - Platform/elevator movement
 * - Teleportation effects
 * - Kinematic animation
 *
 * @param core Engine core reference
 * @param entity Entity with RigidBody component
 * @param velocity Target velocity in m/s (world space)
 *
 * @warning This will activate sleeping bodies
 * @warning Static bodies cannot have their velocity changed
 * @note Frame-rate independent (velocity is constant until changed)
 *
 * @see GetLinearVelocity, AddLinearVelocity
 */
void SetLinearVelocity(Engine::Core &core, Engine::Entity entity, const glm::vec3 &velocity);

/**
 * @brief Get the current linear velocity of a rigid body (world space)
 *
 * Returns the instantaneous linear velocity at the current frame.
 *
 * @param core Engine core reference
 * @param entity Entity with RigidBody component
 * @return Current velocity in m/s (world space), or (0,0,0) if entity invalid
 *
 * @see SetLinearVelocity, AddLinearVelocity
 */
glm::vec3 GetLinearVelocity(Engine::Core &core, Engine::Entity entity);

/**
 * @brief Add a delta to the current linear velocity
 *
 * Convenience function equivalent to:
 *   SetLinearVelocity(GetLinearVelocity() + deltaVelocity)
 *
 * Use cases:
 * - Jump mechanics (add upward velocity)
 * - Dash/boost effects
 * - Knockback/launch effects
 *
 * @param core Engine core reference
 * @param entity Entity with RigidBody component
 * @param deltaVelocity Velocity delta to add in m/s (world space)
 *
 * @see SetLinearVelocity, GetLinearVelocity
 */
void AddLinearVelocity(Engine::Core &core, Engine::Entity entity, const glm::vec3 &deltaVelocity);

// ============================================================================
// ANGULAR VELOCITY CONTROL
// ============================================================================

/**
 * @brief Set the angular velocity of a rigid body (world space)
 *
 * Sets the instantaneous angular velocity (rotation speed).
 * The direction of the vector is the rotation axis, the magnitude is the
 * rotation speed in radians per second.
 *
 * Use cases:
 * - Spinning objects (fans, wheels)
 * - Rotating platforms
 * - Continuous rotation without torque
 *
 * @param core Engine core reference
 * @param entity Entity with RigidBody component
 * @param angularVelocity Angular velocity (axis + magnitude) in rad/s (world space)
 *
 * @warning This will activate sleeping bodies
 * @warning Static bodies cannot have their velocity changed
 * @note Use glm::radians() to convert degrees to radians
 *
 * Example: Spin 90°/sec around Y-axis
 * @code
 * SetAngularVelocity(core, entity, glm::vec3(0, glm::radians(90.0f), 0));
 * @endcode
 *
 * @see GetAngularVelocity, AddAngularVelocity
 */
void SetAngularVelocity(Engine::Core &core, Engine::Entity entity, const glm::vec3 &angularVelocity);

/**
 * @brief Get the current angular velocity of a rigid body (world space)
 *
 * Returns the instantaneous angular velocity at the current frame.
 * The direction is the rotation axis, the magnitude is the speed in rad/s.
 *
 * @param core Engine core reference
 * @param entity Entity with RigidBody component
 * @return Current angular velocity in rad/s (world space), or (0,0,0) if invalid
 *
 * @see SetAngularVelocity, AddAngularVelocity
 */
glm::vec3 GetAngularVelocity(Engine::Core &core, Engine::Entity entity);

/**
 * @brief Add a delta to the current angular velocity
 *
 * Convenience function equivalent to:
 *   SetAngularVelocity(GetAngularVelocity() + deltaAngularVelocity)
 *
 * Use cases:
 * - Spin-up effects (gradually increase spin)
 * - Angular knockback
 * - Rotational boosts
 *
 * @param core Engine core reference
 * @param entity Entity with RigidBody component
 * @param deltaAngularVelocity Angular velocity delta in rad/s (world space)
 *
 * @see SetAngularVelocity, GetAngularVelocity
 */
void AddAngularVelocity(Engine::Core &core, Engine::Entity entity, const glm::vec3 &deltaAngularVelocity);

} // namespace Physics::Helper
