/**************************************************************************
 * EngineSquared v0.1.1
 *
 * EngineSquared is a software package, part of the Engine² organization.
 *
 * This file is part of the EngineSquared project that is under MIT License.
 * Copyright © 2025-present by @EngineSquared, All rights reserved.
 *
 * EngineSquared is a free software: you can redistribute it and/or modify
 * it under the terms of the MIT License. See the project's LICENSE file for
 * the full license text and details.
 *
 * @file WheeledVehicleControllerSettings.hpp
 * @brief Wrapper class for Jolt's WheeledVehicleControllerSettings
 *
 * This file provides a custom wrapper around JPH::WheeledVehicleControllerSettings
 * for use within the EngineSquared physics plugin.
 *
 * @author @EngineSquared
 * @version 0.1.1
 * @date 2025-10-30
 **************************************************************************/

#pragma once

// clang-format off
#include <Jolt/Jolt.h>
// clang-format on

#include <Jolt/Physics/Vehicle/WheeledVehicleController.h>

namespace Physics::Utils {

// Forward declaration
class WheeledVehicleController;

/**
 * @brief Custom wrapper for Jolt's WheeledVehicleControllerSettings
 *
 * This class inherits from JPH::WheeledVehicleControllerSettings and provides
 * a layer of abstraction for vehicle controller configuration within the
 * EngineSquared physics plugin.
 *
 * Currently, this class provides the same functionality as its parent class.
 * It serves as a foundation for future customization and extensions.
 */
class WheeledVehicleControllerSettings : public JPH::WheeledVehicleControllerSettings {
  public:
    /**
     * @brief Default constructor
     */
    WheeledVehicleControllerSettings() = default;

    /**
     * @brief Copy constructor
     * @param other The settings to copy from
     */
    WheeledVehicleControllerSettings(const WheeledVehicleControllerSettings &other) = default;

    /**
     * @brief Copy constructor from JPH::WheeledVehicleControllerSettings
     * @param other The Jolt settings to copy from
     */
    WheeledVehicleControllerSettings(const JPH::WheeledVehicleControllerSettings &other)
        : JPH::WheeledVehicleControllerSettings(other)
    {
    }

    /**
     * @brief Destructor
     */
    ~WheeledVehicleControllerSettings() override = default;

    /**
     * @brief Copy assignment operator
     * @param other The settings to copy from
     * @return Reference to this object
     */
    WheeledVehicleControllerSettings &operator=(const WheeledVehicleControllerSettings &other) = default;

    /**
     * @brief Construct a controller from these settings
     * @param inConstraint The vehicle constraint to attach the controller to
     * @return Pointer to the newly created controller
     */
    JPH::VehicleController *ConstructController(JPH::VehicleConstraint &inConstraint) const override;
};

} // namespace Physics::Utils
