#pragma once

// clang-format off
#include <Jolt/Jolt.h>
// clang-format on

#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Body/MotionType.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/ConvexHullShape.h>
#include <Jolt/Physics/Collision/Shape/OffsetCenterOfMassShape.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <Jolt/Physics/Vehicle/VehicleAntiRollBar.h>
#include <Jolt/Physics/Vehicle/VehicleCollisionTester.h>
#include <Jolt/Physics/Vehicle/VehicleConstraint.h>
#include <Jolt/Physics/Vehicle/VehicleController.h>
#include <Jolt/Physics/Vehicle/VehicleDifferential.h>
#include <Jolt/Physics/Vehicle/VehicleEngine.h>
#include <Jolt/Physics/Vehicle/VehicleTrack.h>
#include <Jolt/Physics/Vehicle/VehicleTransmission.h>
#include <Jolt/Physics/Vehicle/Wheel.h>
#include <Jolt/Physics/Vehicle/WheeledVehicleController.h>
#include <glm/glm.hpp>
#include <memory>

#include "Layers.hpp"

namespace ES::Plugin::Physics::Component {
/// @brief A wheeled vehicle component.
struct WheeledVehicle3D {
    /// @brief The shape settings for the body itself, built from the mesh.
    std::shared_ptr<JPH::ShapeSettings> bodySettings;
    /// @brief The final shape settings of the vehicle.
    std::shared_ptr<JPH::ShapeSettings> finalShapeSettings;
    /// @brief The vehicle constraint settings.
    std::shared_ptr<JPH::VehicleConstraintSettings> vehicleConstraintSettings;
    /// @brief The vehicle controller settings.
    std::shared_ptr<JPH::WheeledVehicleControllerSettings> controllerSettings;
    /// @brief The vehicle constraint itself. This should not be constructed manually.
    std::shared_ptr<JPH::VehicleConstraint> vehicleConstraint;

    /// @brief Sub struct for wheels
    struct Wheel {
        /// @brief The vehicle parent entity.
        ES::Engine::Entity parentVehicle;
        /// @brief The wheel settings.
        std::shared_ptr<JPH::WheelSettingsWV> wheelSettings;
        /// @brief The index of the wheel in the vehicle.
        size_t index;
    };

    /// @brief Send user (driver) input to the controller.
    /// @param inForward The forward input (-1 to 1).
    /// @param inRight The right input (-1 to 1).
    /// @param inBrake The brake input (0 to 1).
    /// @param inHandBrake The handbrake input (0 to 1).
    /// @note Make sure the vehicle is activated before calling this function.
    /// It can deactivate if the vehicle is not moving.
    /// @see PhysicsManager
    void SetDriverInput(float inForward, float inRight, float inBrake, float inHandBrake)
    {
        if (!vehicleConstraint)
        {
            return;
        }

        auto controller = reinterpret_cast<JPH::WheeledVehicleController *>(vehicleConstraint->GetController());
        if (controller)
        {
            controller->SetDriverInput(inForward, inRight, inBrake, inHandBrake);
        }
    }
};
} // namespace ES::Plugin::Physics::Component
