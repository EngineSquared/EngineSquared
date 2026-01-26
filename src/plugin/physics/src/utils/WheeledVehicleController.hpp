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
 * @file WheeledVehicleController.hpp
 * @brief Wrapper class for Jolt's WheeledVehicleController
 *
 * This file provides a custom wrapper around JPH::WheeledVehicleController
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
class WheeledVehicleControllerSettings;

/**
 * @brief Custom wrapper for Jolt's WheeledVehicleController
 *
 * This class inherits from JPH::WheeledVehicleController and provides
 * a layer of abstraction for vehicle controller functionality within the
 * EngineSquared physics plugin.
 *
 * Currently, this class provides the same functionality as its parent class.
 * It serves as a foundation for future customization and extensions.
 */
class WheeledVehicleController : public JPH::WheeledVehicleController {
  public:
    /**
     * @brief Constructor
     * @param inSettings The settings to construct the controller from
     * @param inConstraint The vehicle constraint this controller is attached to
     */
    WheeledVehicleController(const WheeledVehicleControllerSettings &inSettings, JPH::VehicleConstraint &inConstraint);

    /**
     * @brief Destructor
     */
    virtual ~WheeledVehicleController() = default;
};

} // namespace Physics::Utils
