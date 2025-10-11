#pragma once

#include "core/Core.hpp"

namespace Plugin::Physics::System {
/**
 * @brief Init internal elements needed by JoltPhysics.
 *
 * @param core  core
 * @note To be used with the "Startup" scheduler.
 */
void InitJoltPhysics(Engine::Core &core);

} // namespace Plugin::Physics::System
