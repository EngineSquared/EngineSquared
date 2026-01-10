#pragma once

#include "core/Core.hpp"

namespace Physics::System {

/**
 * @brief Initialize the vehicle system with entt lifecycle hooks
 *
 * Registers callbacks for Vehicle component construction and destruction.
 * Must be called during Engine startup (Scheduler::Startup).
 */
void InitVehicleSystem(Engine::Core &core);

} // namespace Physics::System
