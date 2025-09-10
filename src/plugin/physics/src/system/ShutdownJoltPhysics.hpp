#pragma once

#include "core/Core.hpp"

namespace ES::Plugin::Physics::System {
/**
 * @brief Shutdown Jolt's PhysicsSystem.
 *
 * @param core  core
 * @note To be used with the "Startup" scheduler.
 */
void ShutdownJoltPhysics(ES::Engine::Core &core);

} // namespace ES::Plugin::Physics::System
