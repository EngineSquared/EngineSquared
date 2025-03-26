#pragma once

#include "Core.hpp"
#include "RigidBody3D.hpp"

namespace ES::Plugin::Physics::System {
/**
 * @brief Init internal elements needed by JoltPhysics.
 *
 * @param core  core
 * @note To be used with the "Startup" scheduler.
 */
void InitJoltPhysics(ES::Engine::Core &core);

} // namespace ES::Plugin::Physics::System
