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
 * @file RigidBodySystem.hpp
 * @brief System to manage RigidBody lifecycle with entt hooks
 *
 * This system registers entt hooks (on_construct, on_destroy) to automatically
 * create/destroy Jolt physics bodies when RigidBody components are added/removed.
 *
 * @author @EngineSquared
 * @version 0.1.1
 * @date 2025-10-29
 **************************************************************************/

#pragma once

#include "core/Core.hpp"

namespace Physics::System {

/**
 * @brief Initialize RigidBody system and register entt hooks
 *
 * This system sets up the entt hooks for RigidBody component:
 * - on_construct: Creates Jolt body, adds RigidBodyInternal component
 * - on_destroy: Removes Jolt body, cleans up RigidBodyInternal
 *
 * @param core The engine core
 * @note To be used with the "Startup" scheduler
 */
void InitRigidBodySystem(Engine::Core &core);

} // namespace Physics::System
