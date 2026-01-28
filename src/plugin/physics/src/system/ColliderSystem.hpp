/**************************************************************************
 * EngineSquared v0.2.0
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
 * @file ColliderSystem.hpp
 * @brief System to manage Collider lifecycle with entt hooks
 *
 * This system registers entt hooks (on_construct) to automatically
 * create a static RigidBody when a Collider is added to an entity
 * without an existing RigidBody.
 *
 * @author @EngineSquared
 * @version 0.2.0
 * @date 2026-01-25
 **************************************************************************/

#pragma once

#include "core/Core.hpp"

namespace Physics::System {

/**
 * @brief Initialize Collider system and register entt hooks
 *
 * This system sets up the entt hooks for Collider components:
 * - on_construct: Creates a static RigidBody if none exists
 *
 * @param core The engine core
 * @note To be used with the "Startup" scheduler, after InitRigidBodySystem
 */
void InitColliderSystem(Engine::Core &core);

} // namespace Physics::System
