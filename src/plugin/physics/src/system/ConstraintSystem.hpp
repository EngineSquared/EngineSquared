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
 * @file ConstraintSystem.hpp
 * @brief System for managing physics constraints via entt hooks
 *
 * This system automatically creates and destroys Jolt constraints when
 * constraint components are added/removed from entities.
 *
 * @author @EngineSquared
 * @version 0.1.1
 * @date 2025-12-04
 **************************************************************************/

#pragma once

#include "Engine.hpp"

namespace Physics::System {

/**
 * @brief Initialize the constraint system
 *
 * Sets up entt hooks for automatic constraint creation/destruction.
 * Must be called after InitRigidBodySystem and physics manager initialization.
 *
 * Registered hooks:
 * - FixedConstraint: on_construct creates Jolt FixedConstraint
 * - DistanceConstraint: on_construct creates Jolt DistanceConstraint
 * - PointConstraint: on_construct creates Jolt PointConstraint
 * - All constraints: on_destroy removes from Jolt physics system
 *
 * @param core Reference to the engine core
 *
 * @example
 * @code
 * Engine::Core &core = ...;
 *
 * // Initialize physics (must come first)
 * Physics::Resource::PhysicsManager physicsManager;
 * core.AddResource(physicsManager);
 * physicsManager.Init(core);
 *
 * // Initialize body system
 * Physics::System::InitRigidBodySystem(core);
 *
 * // Initialize constraint system
 * Physics::System::InitConstraintSystem(core);
 * @endcode
 */
void InitConstraintSystem(Engine::Core &core);

} // namespace Physics::System
