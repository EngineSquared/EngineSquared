#pragma once

#include "core/Core.hpp"

namespace Physics::System {
/**
 * @brief Applies every update of the physics system.
 *
 * After physics update, flush any buffered contact events from worker threads
 * to the EventManager in the core.
 *
 * @param core  core
 * @note To be used with the "RelativeTimeUpdate" scheduler.
 */
void PhysicsUpdate(Engine::Core &core);
} // namespace Physics::System
