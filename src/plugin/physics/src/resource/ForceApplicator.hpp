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
 * @file ForceApplicator.hpp
 * @brief API for applying forces, torques, and impulses to rigid bodies
 *
 * This API provides a clean interface for applying forces and impulses
 * to entities with RigidBody components. It handles all the necessary
 * conversions and error checking.
 *
 * @author @EngineSquared
 * @version 0.1.1
 * @date 2025-11-05
 **************************************************************************/

#pragma once

#include <Engine.hpp>
#include <glm/glm.hpp>

namespace Physics::Resource {

//============================================================================
// CONTINUOUS FORCES (applied each frame)
//============================================================================

/**
 * @brief Apply a force at the center of mass of a rigid body
 *
 * Forces are continuous and should be applied every frame. They will
 * accelerate the body over time according to F = ma.
 *
 * @param core The engine core
 * @param entity The entity with a RigidBody component
 * @param force The force vector in world space (Newtons)
 *
 * @note This only works on Dynamic bodies. Static and Kinematic bodies will
 *       log a warning and ignore the force.
 *
 * @example Apply upward force (like a thruster):
 * @code
 * glm::vec3 thrustForce(0.0f, 100.0f, 0.0f); // 100N upward
 * Physics::Resource::AddForce(core, rocket, thrustForce);
 * @endcode
 */
void AddForce(Engine::Core &core, Engine::Entity entity, const glm::vec3 &force);

/**
 * @brief Apply a force at a specific point on a rigid body
 *
 * Applying a force off-center will create both linear and angular acceleration.
 * This is useful for simulating impacts, explosions, or propulsion at specific
 * points on an object.
 *
 * @param core The engine core
 * @param entity The entity with a RigidBody component
 * @param force The force vector in world space (Newtons)
 * @param worldPoint The point of application in world space
 *
 * @note If worldPoint is at the center of mass, this is equivalent to AddForce()
 *
 * @example Apply wind force to the side of a sail:
 * @code
 * glm::vec3 windForce(50.0f, 0.0f, 0.0f);
 * glm::vec3 sailPosition = transform.position + glm::vec3(0, 2, 0);
 * Physics::Resource::AddForceAtPoint(core, boat, windForce, sailPosition);
 * @endcode
 */
void AddForceAtPoint(Engine::Core &core, Engine::Entity entity,
                     const glm::vec3 &force,
                     const glm::vec3 &worldPoint);

/**
 * @brief Apply a torque (rotational force) to a rigid body
 *
 * Torque causes pure rotation without translation. The direction of the
 * torque vector determines the axis of rotation (right-hand rule), and
 * its magnitude determines the strength.
 *
 * @param core The engine core
 * @param entity The entity with a RigidBody component
 * @param torque The torque vector in world space (Newton-meters)
 *
 * @example Spin a wheel around its Y axis:
 * @code
 * glm::vec3 spinTorque(0.0f, 10.0f, 0.0f); // 10 Nm around Y
 * Physics::Resource::AddTorque(core, wheel, spinTorque);
 * @endcode
 */
void AddTorque(Engine::Core &core, Engine::Entity entity, const glm::vec3 &torque);

//============================================================================
// INSTANTANEOUS IMPULSES (one-time application)
//============================================================================

/**
 * @brief Apply an instantaneous impulse at the center of mass
 *
 * Impulses change velocity immediately (in one physics step). They are
 * useful for instantaneous events like jumping, explosions, or collisions.
 *
 * @param core The engine core
 * @param entity The entity with a RigidBody component
 * @param impulse The impulse vector in world space (Newton-seconds or kg⋅m/s)
 *
 * @note Unlike forces, impulses are not framerate-dependent and should only
 *       be applied once per event.
 *
 * @example Make a character jump:
 * @code
 * glm::vec3 jumpImpulse(0.0f, 300.0f, 0.0f); // 300 kg⋅m/s upward
 * Physics::Resource::AddImpulse(core, player, jumpImpulse);
 * @endcode
 */
void AddImpulse(Engine::Core &core, Engine::Entity entity, const glm::vec3 &impulse);

/**
 * @brief Apply an instantaneous impulse at a specific point
 *
 * Applying an impulse off-center will create both linear and angular velocity
 * changes. This is ideal for simulating explosions, bullet impacts, or other
 * instantaneous forces at specific points.
 *
 * @param core The engine core
 * @param entity The entity with a RigidBody component
 * @param impulse The impulse vector in world space (Newton-seconds or kg⋅m/s)
 * @param worldPoint The point of application in world space
 *
 * @example Explosion pushing objects away from center:
 * @code
 * glm::vec3 explosionCenter(0, 0, 0);
 * glm::vec3 toObject = transform.position - explosionCenter;
 * glm::vec3 impulse = glm::normalize(toObject) * 500.0f;
 * Physics::Resource::AddImpulseAtPoint(core, object, impulse, transform.position);
 * @endcode
 */
void AddImpulseAtPoint(Engine::Core &core, Engine::Entity entity,
                       const glm::vec3 &impulse,
                       const glm::vec3 &worldPoint);

/**
 * @brief Apply an instantaneous angular impulse (rotational impulse)
 *
 * Angular impulses change angular velocity immediately, causing instantaneous
 * spin changes. The direction of the vector determines the axis of rotation
 * (right-hand rule), and its magnitude determines the strength.
 *
 * @param core The engine core
 * @param entity The entity with a RigidBody component
 * @param angularImpulse The angular impulse vector in world space (kg⋅m²/s)
 *
 * @example Instantly spin a coin when flipped:
 * @code
 * glm::vec3 flipImpulse(0.0f, 50.0f, 0.0f); // 50 kg⋅m²/s around Y
 * Physics::Resource::AddAngularImpulse(core, coin, flipImpulse);
 * @endcode
 */
void AddAngularImpulse(Engine::Core &core, Engine::Entity entity,
                       const glm::vec3 &angularImpulse);

} // namespace Physics::Resource
