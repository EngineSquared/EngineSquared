#pragma once

#include "Core.hpp"

namespace ES::Plugin::Physics::System {
/**
 * @brief Applies every update of the physics system.
 *
 * @param registry  registry
 * @note To be used with the "RelativeTimeUpdate" scheduler.
 */
void PhysicsUpdate(ES::Engine::Core &registry);
} // namespace ES::Plugin::Physics::System
