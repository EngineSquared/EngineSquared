#include "Physics.pch.hpp"

#include "system/VehicleControlSystem.hpp"

#include "component/Vehicle.hpp"
#include "component/VehicleController.hpp"
#include "component/VehicleInternal.hpp"
#include "resource/PhysicsManager.hpp"

#include <Jolt/Physics/Vehicle/WheeledVehicleController.h>

namespace Physics::System {

void VehicleControlSystem(Engine::Core &core)
{
    auto &registry = core.GetRegistry();
    auto &physicsManager = core.GetResource<Resource::PhysicsManager>();

    if (!physicsManager.IsPhysicsActivated())
        return;

    auto &bodyInterface = physicsManager.GetPhysicsSystem().GetBodyInterface();

    registry.view<Component::VehicleController, Component::VehicleInternal>().each(
        [&](const Component::VehicleController &controller, const Component::VehicleInternal &internal) {
            if (!internal.IsValid() || !internal.vehicleController)
                return;

            auto *wheeledController = internal.vehicleController;

            wheeledController->SetDriverInput(controller.forwardInput, controller.steeringInput, controller.brakeInput,
                                              controller.handBrakeInput);

            if (controller.forwardInput != 0.0f || controller.steeringInput != 0.0f || controller.brakeInput != 0.0f ||
                controller.handBrakeInput != 0.0f)
            {
                bodyInterface.ActivateBody(internal.chassisBodyID);
            }
        });
}

} // namespace Physics::System
