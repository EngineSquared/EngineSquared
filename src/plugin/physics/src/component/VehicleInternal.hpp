#pragma once

#include "entity/Entity.hpp"
#include <Jolt/Physics/Body/BodyID.h>
#include <Jolt/Physics/Vehicle/VehicleConstraint.h>
#include <Jolt/Physics/Vehicle/VehicleCollisionTester.h>
#include <Jolt/Physics/Vehicle/WheeledVehicleController.h>
#include <array>
#include <memory>

namespace Physics::Component {

/**
 * @brief Internal vehicle component storing Jolt-specific data
 *
 * This component is automatically managed by the VehicleSystem.
 * Users should NEVER directly create or modify this component.
 *
 * Stores the vehicle constraint, controller, and wheel body references.
 */
struct VehicleInternal {
    /// The Jolt vehicle constraint (owns the vehicle physics)
    /// Raw pointer - manually managed, must be removed from PhysicsSystem before deletion
    JPH::VehicleConstraint* vehicleConstraint = nullptr;

    /// The Jolt vehicle controller (manages input/control)
    /// Raw pointer - owned by vehicleConstraint, do not delete separately
    JPH::WheeledVehicleController* vehicleController = nullptr;

    /// Collision tester for vehicle wheel raycasts (must stay alive while constraint exists)
    /// Using Jolt's Ref smart pointer for proper reference counting
    JPH::Ref<JPH::VehicleCollisionTester> collisionTester;

    /// Entity IDs of the 4 wheel entities (for transform sync)
    std::array<Engine::Entity, 4> wheelEntities;

    /// Jolt BodyIDs for the 4 wheel collision bodies
    std::array<JPH::BodyID, 4> wheelBodyIDs;

    /// BodyID of the chassis
    JPH::BodyID chassisBodyID;

    /**
     * @brief Check if the vehicle constraint is valid
     */
    bool IsValid() const
    {
        return vehicleConstraint != nullptr && !chassisBodyID.IsInvalid();
    }

    /**
     * @brief Default constructor
     */
    VehicleInternal() = default;

    /**
     * @brief Construct with chassis body ID
     */
    explicit VehicleInternal(JPH::BodyID chassis) : chassisBodyID(chassis) {}
};

} // namespace Physics::Component
