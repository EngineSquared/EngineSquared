#pragma once

#include "core/Core.hpp"

namespace Physics::System {
/**
 * @brief Init the PhysicsManager.
 *
 * @param core  core
 * @note To be used with the "Startup" scheduler.
 */
void InitPhysicsManager(Engine::Core &core);

} // namespace Physics::System
