#pragma once

#include "core/Core.hpp"

namespace ES::Plugin::Physics::System {
/**
 * @brief Init the PhysicsManager.
 *
 * @param core  core
 * @note To be used with the "Startup" scheduler.
 */
void InitPhysicsManager(ES::Engine::Core &core);

} // namespace ES::Plugin::Physics::System
