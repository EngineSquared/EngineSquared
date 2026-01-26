#include "Physics.pch.hpp"

#include "system/VehicleRPMUpdate.hpp"

#include "component/VehicleInternal.hpp"
#include "resource/PhysicsManager.hpp"
#include "resource/VehicleTelemetry.hpp"

#include "utils/WheeledVehicleController.hpp"

namespace Physics::System {

void VehicleRPMUpdate(Engine::Core &core)
{
    auto &registry = core.GetRegistry();
    auto &physicsManager = core.GetResource<Resource::PhysicsManager>();

    if (!physicsManager.IsPhysicsActivated())
        return;

    auto &telemetry = core.GetResource<Resource::VehicleTelemetry>();

    auto view = registry.view<Component::VehicleInternal>();
    for (auto [e, internal] : view.each())
    {
        if (!internal.IsValid() || !internal.vehicleConstraint)
            continue;

        auto *controller = static_cast<Utils::WheeledVehicleController *>(internal.vehicleConstraint->GetController());
        if (!controller)
            continue;

        float rpm = controller->GetEngine().GetCurrentRPM();
        if (rpm < 0.0f)
            rpm = 0.0f;

        Engine::EntityId eid{static_cast<Engine::EntityId::ValueType>(e)};
        telemetry.SetRPM(eid, rpm);
    }
}

} // namespace Physics::System
