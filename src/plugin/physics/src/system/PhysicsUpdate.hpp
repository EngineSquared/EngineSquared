#pragma once

#include "core/Core.hpp"

namespace Physics::System {
/**
 * @brief Applies every update of the physics system.
 *
 * @param core  core
 * @note To be used with the "RelativeTimeUpdate" scheduler.
 */
void PhysicsUpdate(Engine::Core &core);
} // namespace Physics::System
