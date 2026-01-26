#pragma once

#include "core/Core.hpp"

namespace Physics::System {

/**
 * @brief Update per-vehicle RPM telemetry from Jolt controller.
 * Runs in FixedTimeUpdate.
 */
void VehicleRPMUpdate(Engine::Core &core);

} // namespace Physics::System
