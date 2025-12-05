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
 * @file SoftBodySystem.hpp
 * @brief System for managing soft body physics
 *
 * Handles creation, destruction, and synchronization of soft bodies
 * with the Jolt physics engine.
 *
 * @author @EngineSquared
 * @version 0.1.1
 * @date 2025-12-05
 **************************************************************************/

#pragma once

namespace Engine {
class Core;
}

namespace Physics::System {

/**
 * @brief Initialize the soft body system
 *
 * Registers entt hooks for SoftBody component creation and destruction.
 * Should be called during physics plugin initialization.
 *
 * @param core Reference to the engine core
 */
void InitSoftBodySystem(Engine::Core &core);

/**
 * @brief Shutdown the soft body system
 *
 * Cleans up any remaining soft bodies.
 *
 * @param core Reference to the engine core
 */
void ShutdownSoftBodySystem(Engine::Core &core);

/**
 * @brief Synchronize soft body vertex positions to mesh data
 *
 * Updates vertex positions from Jolt soft body simulation.
 * Should be called after physics update.
 *
 * @param core Reference to the engine core
 */
void SyncSoftBodyVertices(Engine::Core &core);

} // namespace Physics::System
