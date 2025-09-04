#pragma once

#include "component/RigidBody3D.hpp"
#include "core/Core.hpp"

namespace ES::Plugin::Physics::System {
/**
 * @brief Init internal elements needed by JoltPhysics.
 *
 * @param core  core
 * @note To be used with the "Startup" scheduler.
 */
void InitJoltPhysics(ES::Engine::Core &core);

} // namespace ES::Plugin::Physics::System
