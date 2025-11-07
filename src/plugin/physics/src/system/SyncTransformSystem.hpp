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
 * @file SyncTransformSystem.hpp
 * @brief System to synchronize Transform with Jolt Physics body positions
 *
 * This system runs after PhysicsUpdate to copy the simulated positions
 * and rotations from Jolt bodies back to the Transform components.
 *
 * @author @EngineSquared
 * @version 0.1.1
 * @date 2025-10-30
 **************************************************************************/

#pragma once

#include "core/Core.hpp"

namespace Physics::System {

/**
 * @brief Synchronize Transform components with Jolt Physics body positions
 *
 * This system should run in the FixedTimeUpdate scheduler AFTER PhysicsUpdate.
 * It reads the position and rotation from Jolt bodies and updates the
 * corresponding Transform components.
 *
 * Only dynamic and kinematic bodies are synchronized (static bodies don't move).
 *
 * @param core The engine core
 * @note To be used with the "FixedTimeUpdate" scheduler
 */
void SyncTransformWithPhysics(Engine::Core &core);

} // namespace Physics::System
