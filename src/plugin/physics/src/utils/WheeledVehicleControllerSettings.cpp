#include "Physics.pch.hpp"

#include "utils/WheeledVehicleController.hpp"
#include "utils/WheeledVehicleControllerSettings.hpp"

namespace Physics::Utils {

JPH::VehicleController *
WheeledVehicleControllerSettings::ConstructController(JPH::VehicleConstraint &inConstraint) const
{
    // Safety: JoltPhysics will cast it to a reference-counted pointer through its internal system
    return new WheeledVehicleController(*this, inConstraint); // NOSONAR
}

} // namespace Physics::Utils
