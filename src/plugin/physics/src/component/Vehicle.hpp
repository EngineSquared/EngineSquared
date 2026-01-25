#pragma once

#include "component/WheelSettings.hpp"
#include "entity/EntityId.hpp"
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
 * @brief Collision tester type for vehicle wheel collision detection
 *
 * Determines how wheel-ground collision is detected.
 * CastCylinder is recommended for most use cases as it handles
 * internal edges between adjacent static bodies (e.g., floor tiles) better.
 */
enum class CollisionTesterType : uint8_t {
    /// Simple raycast - fastest but prone to ghost collisions on tiled floors
    Ray,
    /// Sphere cast - better than ray, good for rough terrain
    CastSphere,
    /// Cylinder cast - most accurate, best for tiled floors and complex terrain (default)
    CastCylinder
};

/**
 * @brief Transmission mode - how gears are shifted
 */
enum class TransmissionMode : uint8_t {
    /// Automatically shift gear up and down
    Auto,
    /// Manual gear shift (controlled by user input)
    Manual
};

/**
 * @brief Torque curve point for normalized engine torque
 *
 * Defines a point on the torque curve:
 * - X-axis (rpm): Fraction of engine RPM (0 = minRPM, 1 = maxRPM)
 * - Y-axis (torque): Ratio of max torque (0 = 0 Nm, 1 = maxTorque)
 */
struct TorqueCurvePoint {
    float rpm;    ///< Normalized RPM (0.0 to 1.0)
    float torque; ///< Normalized torque (0.0 to 1.0)
};

/**
 * @brief Gearbox configuration for vehicle transmission
 */
struct GearboxSettings {
    /// Transmission mode (auto or manual shifting)
    TransmissionMode mode = TransmissionMode::Auto;

    /// Forward gear ratios (rotation rate between engine and gearbox)
    /// Index 0 = 1st gear, 1 = 2nd gear, etc.
    std::vector<float> forwardGearRatios = {2.66f, 1.78f, 1.3f, 1.0f, 0.74f};

    /// Reverse gear ratios (rotation rate between engine and gearbox when in reverse)
    /// Typically negative values. Index 0 = 1st reverse gear, etc.
    std::vector<float> reverseGearRatios = {-2.90f};

    /// Current gear (-1 = reverse, 0 = neutral, 1 = first forward, etc.)
    int currentGear = 1;

    /// How long it takes to switch gears (s) - only used in auto mode
    float switchTime = 0.5f;

    /// How long it takes to release the clutch (go to full friction) - only used in auto mode
    float clutchReleaseTime = 0.3f;

    /// How long to wait after releasing clutch before another switch is attempted (s) - only used in auto mode
    float switchLatency = 0.5f;

    /// If RPM of engine is bigger than this, shift a gear up - only used in auto mode
    float shiftUpRPM = 4000.0f;

    /// If RPM of engine is smaller than this, shift a gear down - only used in auto mode
    float shiftDownRPM = 2000.0f;

    /// Strength of the clutch when fully engaged
    /// Total torque = ClutchStrength * (Velocity Engine - Avg Velocity Wheels At Clutch)
    /// Units: kg * m^2 * s^-1
    float clutchStrength = 10.0f;

    /// Current clutch friction (0 = no friction, 1 = full friction) - for manual mode
    float clutchFriction = 1.0f;
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

    /// Normalized torque curve - describes torque output across RPM range
    /// Y-axis: Ratio of max torque (0.0 = 0 Nm, 1.0 = maxTorque)
    /// X-axis: Fraction of RPM (0.0 = minRPM, 1.0 = maxRPM)
    /// Default: realistic curve with peak torque at mid-range RPM
    std::vector<TorqueCurvePoint> normalizedTorque = {
        {0.0f,  0.8f}, // 80% torque at minRPM
        {0.66f, 1.0f}, // 100% torque at 66% of RPM range (peak)
        {1.0f,  0.8f}  // 80% torque at maxRPM
    };

    /// Moment of inertia (kg m^2) of the engine
    float inertia = 0.5f;

    /// Angular damping factor: dw/dt = -c * w
    /// Value should be zero or positive, usually close to 0
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

    /// Wheel entities for visual representation
    std::array<Engine::EntityId, 4> wheelEntities;

    /// Wheel positions relative to chassis center
    std::array<glm::vec3, 4> wheelPositions = GetDefaultWheelPositions();

    /// Collision tester type for wheel-ground detection (default: CastCylinder)
    CollisionTesterType collisionTesterType = CollisionTesterType::CastCylinder;

    /// Convex radius fraction for CastCylinder tester (0.0 to 1.0)
    /// Higher values help prevent ghost collisions (default: 0.5, increase if needed)
    float convexRadiusFraction = 0.5f;

    /**
     * @brief Get default wheel positions relative to chassis center
     *
     * Default positions assume a standard car layout:
     * - Front wheels: ±0.9 X, +1.2 Z
     * - Rear wheels: ±0.9 X, -1.2 Z
     * - All wheels: -0.3 Y (below chassis center)
     *
     * @return Array of wheel positions indexed by WheelIndex
     */
    static std::array<glm::vec3, 4> GetDefaultWheelPositions()
    {
        return {
            glm::vec3(-0.9f, -0.3f, 1.2f),  // Front-left
            glm::vec3(0.9f, -0.3f, 1.2f),   // Front-right
            glm::vec3(-0.9f, -0.3f, -1.2f), // Rear-left
            glm::vec3(0.9f, -0.3f, -1.2f)   // Rear-right
        };
    }

    /**
     * @brief Create a default RWD sports car configuration
     */
    static Vehicle CreateDefaultCar()
    {
        using enum WheelIndex;

        Vehicle vehicle;
        vehicle.drivetrain = DrivetrainType::RWD;
        vehicle.engine.maxTorque = 600.0f;
        vehicle.engine.maxRPM = 7000.0f;
        vehicle.rollbar.rearStiffness = 8000.0f;

        // Front wheels with steering
        vehicle.wheels[static_cast<size_t>(FrontLeft)] = WheelSettings::CreateFrontWheel();
        vehicle.wheels[static_cast<size_t>(FrontRight)] = WheelSettings::CreateFrontWheel();

        // Rear wheels without steering
        vehicle.wheels[static_cast<size_t>(RearLeft)] = WheelSettings::CreateRearWheel();
        vehicle.wheels[static_cast<size_t>(RearRight)] = WheelSettings::CreateRearWheel();
        return vehicle;
    }
};

} // namespace Physics::Component
