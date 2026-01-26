#include "Physics.pch.hpp"

#include "utils/WheeledVehicleController.hpp"
#include "utils/WheeledVehicleControllerSettings.hpp"

namespace Physics::Utils {

JPH::VehicleController *
WheeledVehicleControllerSettings::ConstructController(JPH::VehicleConstraint &inConstraint) const
{
    return new WheeledVehicleController(*this, inConstraint);
}

} // namespace Physics::Utils
