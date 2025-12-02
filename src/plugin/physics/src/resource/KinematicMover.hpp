/*
** EPITECH PROJECT, 2025
** EngineSquared - Physics Plugin
** File description:
** KinematicMover - API for kinematic body movement (Issue #003)
*/

#pragma once

#include "core/Core.hpp"
#include "entity/Entity.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Physics::Resource {

// ============================================================================
// KINEMATIC MOVEMENT
// ============================================================================

/**
 * @brief Move a kinematic rigid body to a target position and rotation
 *
 * This is the recommended way to move kinematic bodies (platforms, doors,
 * elevators). The body will smoothly interpolate to the target over deltaTime,
 * properly handling collisions and pushing dynamic bodies.
 *
 * Kinematic bodies:
 * - Move under program control (not affected by forces/gravity)
 * - Collide with and push dynamic bodies
 * - Don't respond to collisions themselves
 * - Ideal for gameplay elements (platforms, doors, etc.)
 *
 * Use cases:
 * - Moving platforms
 * - Elevators
 * - Rotating doors
 * - Conveyor belts
 * - Any predictable, controlled movement
 *
 * @param core Engine core reference
 * @param entity Entity with Kinematic RigidBody component
 * @param targetPosition Target position in world space (meters)
 * @param targetRotation Target rotation as quaternion (world space)
 * @param deltaTime Time over which to perform the movement (seconds)
 *
 * @warning Entity must have MotionType::Kinematic
 * @warning deltaTime must be > 0
 * @warning Very large deltaTime may cause tunneling
 *
 * @note This properly handles collision and pushing of dynamic bodies
 * @note Preferred over SetPosition() which can cause tunneling
 *
 * @see SetKinematicVelocity, GetKinematicTarget
 */
void MoveKinematic(Engine::Core &core, Engine::Entity entity, const glm::vec3 &targetPosition,
                   const glm::quat &targetRotation, float deltaTime);

/**
 * @brief Set kinematic body to move with constant velocity
 *
 * Helper function that calculates the next target position based on velocity
 * and calls MoveKinematic(). Useful for constant-speed movement like
 * conveyor belts or simple platforms.
 *
 * Internally:
 * - Gets current position/rotation
 * - Calculates targetPosition = currentPosition + velocity * deltaTime
 * - Calls MoveKinematic() with calculated target
 *
 * Use cases:
 * - Conveyor belts (constant horizontal movement)
 * - Simple linear platforms
 * - Continuous rotation (e.g., fans, gears)
 *
 * @param core Engine core reference
 * @param entity Entity with Kinematic RigidBody component
 * @param velocity Linear velocity in m/s (world space)
 * @param deltaTime Time step (seconds)
 *
 * @warning Entity must have MotionType::Kinematic
 * @warning For complex paths, use MoveKinematic() directly
 *
 * @note Rotation is preserved (use MoveKinematic for rotation control)
 * @note Less flexible than MoveKinematic but simpler for basic cases
 *
 * @see MoveKinematic
 */
void SetKinematicVelocity(Engine::Core &core, Engine::Entity entity, const glm::vec3 &velocity, float deltaTime);

// ============================================================================
// KINEMATIC STATE QUERY (Optional - for debugging)
// ============================================================================

/**
 * @brief Information about a kinematic body's current movement target
 */
struct KinematicTarget {
    glm::vec3 position{0.0f};       ///< Target position (world space)
    glm::quat rotation{1, 0, 0, 0}; ///< Target rotation (world space)
    bool hasTarget = false;         ///< Whether a target is currently set
};

/**
 * @brief Get the kinematic body's current movement target
 *
 * Returns the target position/rotation that the kinematic body is moving
 * towards. Useful for debugging and visualizing movement paths.
 *
 * @param core Engine core reference
 * @param entity Entity with Kinematic RigidBody component
 * @return Current kinematic target, or hasTarget=false if none
 *
 * @note This is primarily for debugging/visualization
 * @note Returns hasTarget=false for non-kinematic bodies
 *
 * @see MoveKinematic
 */
KinematicTarget GetKinematicTarget(Engine::Core &core, Engine::Entity entity);

} // namespace Physics::Resource
