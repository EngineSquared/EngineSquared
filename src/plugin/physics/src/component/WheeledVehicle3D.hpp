#pragma once

// clang-format off
#include <Jolt/Jolt.h>
// clang-format on

#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Body/MotionType.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
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
    /// @brief The final shape settings of the vehicle, which is a OffsetCenterOfMassShape.
    std::shared_ptr<JPH::ShapeSettings> finalShapeSettings;
    /// @brief The vehicle constraint, which is a WheeledVehicleController.
    std::shared_ptr<JPH::VehicleConstraintSettings> vehicleConstraint;
};
} // namespace ES::Plugin::Physics::Component
