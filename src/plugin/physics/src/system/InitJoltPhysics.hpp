#pragma once

#include "core/Core.hpp"

namespace Physics::System {
/**
 * @brief Init internal elements needed by JoltPhysics.
 *
 * @param core  core
 * @note To be used with the "Startup" scheduler.
 */
void InitJoltPhysics(Engine::Core &core);

} // namespace Physics::System
