#pragma once

#include "component/WheelSettings.hpp"
#include <Jolt/Physics/Vehicle/VehicleAntiRollBar.h>
#include <Jolt/Physics/Vehicle/VehicleConstraint.h>
#include <array>
#include <memory>

namespace Physics::Component {

/**
 * @brief Drivetrain configuration for vehicle
 */
enum class DrivetrainType : uint8_t {
    /// All-Wheel Drive - power to all 4 wheels
    AWD,
    /// Front-Wheel Drive - power to front wheels only
    FWD,
    /// Rear-Wheel Drive - power to rear wheels only
    RWD
};

/**
 * @brief Gearbox configuration for vehicle transmission
 */
struct GearboxSettings {
    /// Gear ratios (index 0 is reverse, 1+ are forward gears)
    std::vector<float> gearRatios = {-2.5f, 2.8f, 1.8f, 1.3f, 1.0f, 0.8f};

    /// Current gear index (0 = reverse, 1 = first, etc.)
    int currentGear = 1;

    /// Time to switch gears in seconds
    float clutchReleaseTime = 0.3f;

    /// Gear shift delay
    float switchTime = 0.5f;

    /// Ratio between engine and clutch when clutch is pressed
    float clutchStrength = 10.0f;
};

/**
 * @brief Engine configuration
 */
struct EngineSettings {
    /// Maximum engine torque in Nm
    float maxTorque = 500.0f;

    /// Minimum RPM
    float minRPM = 1000.0f;

    /// Maximum RPM
    float maxRPM = 6000.0f;

    /// Inertia of engine and wheels
    float inertia = 0.5f;

    /// Engine friction (Nm/rad/s)
    float angularDamping = 0.2f;
};

/**
 * @brief Rollbar (anti-roll bar) configuration
 */
struct RollbarSettings {
    /// Front rollbar stiffness (N/rad)
    float frontStiffness = 5000.0f;

    /// Rear rollbar stiffness (N/rad)
    float rearStiffness = 5000.0f;
};

/**
 * @brief Main vehicle component containing all configuration
 *
 * This is the user-facing component that defines vehicle behavior.
 * Attach this to the chassis entity to create a drivable vehicle.
 */
struct Vehicle {
    /// Wheel settings for all 4 wheels (indexed by WheelIndex)
    std::array<WheelSettings, static_cast<size_t>(WheelIndex::Count)> wheels;

    /// Drivetrain configuration
    DrivetrainType drivetrain = DrivetrainType::RWD;

    /// Engine configuration
    EngineSettings engine;

    /// Gearbox configuration
    GearboxSettings gearbox;

    /// Rollbar configuration
    RollbarSettings rollbar;

    /**
     * @brief Create a default RWD sports car configuration
     */
    static Vehicle CreateDefaultCar()
    {
        Vehicle vehicle;
        vehicle.drivetrain = DrivetrainType::RWD;
        vehicle.engine.maxTorque = 600.0f;
        vehicle.engine.maxRPM = 7000.0f;
        vehicle.rollbar.rearStiffness = 8000.0f;

        // Front wheels with steering
        vehicle.wheels[static_cast<size_t>(WheelIndex::FrontLeft)] = WheelSettings::CreateFrontWheel();
        vehicle.wheels[static_cast<size_t>(WheelIndex::FrontRight)] = WheelSettings::CreateFrontWheel();

        // Rear wheels without steering
        vehicle.wheels[static_cast<size_t>(WheelIndex::RearLeft)] = WheelSettings::CreateRearWheel();
        vehicle.wheels[static_cast<size_t>(WheelIndex::RearRight)] = WheelSettings::CreateRearWheel();

        return vehicle;
    }
};

} // namespace Physics::Component
