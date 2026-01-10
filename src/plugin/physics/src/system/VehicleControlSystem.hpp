#pragma once

#include "core/Core.hpp"

namespace Physics::System {

/**
 * @brief Apply vehicle controller inputs to Jolt vehicle constraints
 *
 * Should run every physics tick (FixedTimeUpdate scheduler).
 * Reads VehicleController component values and applies them to the
 * WheeledVehicleController.
 */
void VehicleControlSystem(Engine::Core &core);

} // namespace Physics::System
