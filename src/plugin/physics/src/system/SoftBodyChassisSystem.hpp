/**************************************************************************
 * EngineSquared v0.2.0
 *
 * EngineSquared is a software package, part of the Engine² organization.
 *
 * This file is part of the EngineSquared project that is under MIT License.
 * Copyright © 2024-present by @EngineSquared, All rights reserved.
 *
 * EngineSquared is a free software: you can redistribute it and/or modify
 * it under the terms of the MIT License. See the project's LICENSE file for
 * the full license text and details.
 *
 * @file SoftBodyChassisSystem.hpp
 * @brief System for managing soft body chassis physics.
 *
 * Handles creation, destruction, and synchronization of deformable vehicle
 * chassis using the hybrid RigidBody skeleton + SoftBody visual architecture.
 *
 * @author @EngineSquared
 * @version 0.2.0
 * @date 2026-01-29
 **************************************************************************/

#pragma once

#include "core/Core.hpp"

namespace Physics::System {

/**
 * @brief Initialize the soft body chassis system
 *
 * Registers EnTT hooks for SoftBodyChassis component creation and destruction.
 * This system creates a Jolt SoftBody that uses skinned constraints to follow
 * an invisible skeleton RigidBody (created and managed by the VehicleBuilder/
 * VehicleSystem). If the provided mesh exceeds the configured vertex budget
 * the mesh will be simplified; when simplification fails and
 * `settings.fallbackToRigidBody` is true the component will deactivate itself
 * and fall back to a standard RigidBody-based chassis.
 *
 * This should be called during physics plugin initialization, after
 * VehicleSystem.
 *
 * @param core Reference to the engine core
 */
void InitSoftBodyChassisSystem(Engine::Core &core);

/**
 * @brief Shutdown the soft body chassis system
 *
 * Cleans up any remaining soft body chassis entities.
 *
 * @param core Reference to the engine core
 */
void ShutdownSoftBodyChassisSystem(Engine::Core &core);

/**
 * @brief Synchronize soft body chassis with skeleton rigid body
 *
 * Updates the skinned constraint joint matrices from the skeleton RigidBody's
 * world transform and calls `SkinVertices()` to update soft body vertex
 * positions. This must run during `FixedTimeUpdate` after the physics step
 * (`PhysicsUpdate`) and after transforms from physics have been synchronized
 * to entities (e.g., after `SyncTransformWithPhysics`). The first frame after
 * creation performs a hard skin to align the SoftBody with the skeleton.
 *
 * @param core Reference to the engine core
 */
void SyncSoftBodyChassisWithSkeleton(Engine::Core &core);

/**
 * @brief Synchronize soft body chassis mesh vertices
 *
 * Copies vertex positions from the Jolt `SoftBodyMotionProperties` back into
 * the entity's `Mesh` component and recalculates normals for correct lighting.
 * This should be called after `SyncSoftBodyChassisWithSkeleton`.
 *
 * @param core Reference to the engine core
 */
void SyncSoftBodyChassisMesh(Engine::Core &core);

} // namespace Physics::System
