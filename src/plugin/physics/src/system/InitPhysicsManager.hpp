#pragma once

#include "core/Core.hpp"

namespace Plugin::Physics::System {
/**
 * @brief Init the PhysicsManager.
 *
 * @param core  core
 * @note To be used with the "Startup" scheduler.
 */
void InitPhysicsManager(Engine::Core &core);

} // namespace Plugin::Physics::System
