#include "Physics.pch.hpp"

#include "utils/WheeledVehicleController.hpp"
#include "utils/WheeledVehicleControllerSettings.hpp"

namespace Physics::Utils {

WheeledVehicleController::WheeledVehicleController(const WheeledVehicleControllerSettings &inSettings,
                                                   JPH::VehicleConstraint &inConstraint)
    : JPH::WheeledVehicleController(inSettings, inConstraint)
{
}

} // namespace Physics::Utils
