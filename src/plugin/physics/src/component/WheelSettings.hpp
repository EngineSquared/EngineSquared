#pragma once

#include <cstdint>

namespace Physics::Component {

/**
 * @brief Enum for identifying wheel positions in a 4-wheel vehicle
 */
enum class WheelIndex : uint8_t {
    FrontLeft = 0,
    FrontRight = 1,
    RearLeft = 2,
    RearRight = 3,
    Count = 4
};

/**
 * @brief Settings for an individual wheel in a vehicle
 *
 * Defines physical properties of a single wheel including suspension,
 * dimensions, and friction characteristics.
 */
struct WheelSettings {
    /// Wheel radius in meters
    float radius = 0.3f;

    /// Wheel width in meters (for cylinder collider)
    float width = 0.1f;

    /// Maximum suspension extension length in meters
    float suspensionMaxLength = 0.3f;

    /// Minimum suspension compression length in meters (usually 0)
    float suspensionMinLength = 0.0f;

    /// Suspension spring frequency in Hz (typically 1.0-2.0 for cars)
    /// Higher values = stiffer suspension
    float suspensionFrequency = 1.5f;

    /// Suspension damping ratio (0 = no damping, 1 = critical damping)
    /// Typical car values are 0.3-0.7
    float suspensionDamping = 0.5f;

    /// Maximum steering angle in radians (0 for non-steered wheels)
    float maxSteerAngle = 0.0f;

    /// Lateral (sideways) friction coefficient
    float lateralFriction = 1.5f;

    /// Longitudinal (forward/back) friction coefficient
    float longitudinalFriction = 1.8f;

    /**
     * @brief Create default front wheel settings with steering
     */
    static WheelSettings CreateFrontWheel()
    {
        WheelSettings settings;
        settings.maxSteerAngle = 0.52f; // ~30 degrees
        return settings;
    }

    /**
     * @brief Create default rear wheel settings without steering
     */
    static WheelSettings CreateRearWheel()
    {
        WheelSettings settings;
        settings.maxSteerAngle = 0.0f;
        return settings;
    }
};

} // namespace Physics::Component
