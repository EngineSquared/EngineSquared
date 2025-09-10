#pragma once

#include "core/Core.hpp"

namespace ES::Plugin::Physics::System {
/**
 * @brief Applies every update of the physics system.
 *
 * @param core  core
 * @note To be used with the "RelativeTimeUpdate" scheduler.
 */
void PhysicsUpdate(ES::Engine::Core &core);
} // namespace ES::Plugin::Physics::System
