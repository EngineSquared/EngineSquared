#pragma once

#include <cstdint>
#include <vector>

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
 * @brief Friction curve point for tire friction modeling
 *
 * Defines friction behavior at different slip levels:
 * - Longitudinal: slip ratio (0 = perfect traction, 1 = wheel locked)
 * - Lateral: slip angle in degrees (angle between velocity and wheel direction)
 */
struct FrictionCurvePoint {
    float slip;     ///< Slip ratio (longitudinal) or slip angle in degrees (lateral)
    float friction; ///< Friction coefficient (0 = no friction, >1 = enhanced grip)
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

    /// Moment of inertia (kg m^2) of the wheel
    /// For a cylinder: I = 0.5 * M * R^2
    /// Example: 0.9 for wheel with mass 20kg and radius 0.3m
    float inertia = 0.9f;

    /// Angular damping factor: dw/dt = -c * w
    /// Value should be zero or positive, usually close to 0
    float angularDamping = 0.2f;

    /// Longitudinal friction curve (forward/backward direction)
    /// X-axis: Slip ratio = (omega_wheel * r_wheel - v_longitudinal) / |v_longitudinal|
    /// Y-axis: Friction coefficient (typically 0-1.2)
    /// Default: realistic tire slip curve with peak at 6% slip
    std::vector<FrictionCurvePoint> longitudinalFriction = {
        {0.0f,  0.0f}, // No friction when no slip
        {0.06f, 1.2f}, // Peak friction at 6% slip
        {0.2f,  1.0f}  // Reduced friction when sliding
    };

    /// Lateral friction curve (sideways/slip angle direction)
    /// X-axis: Slip angle in degrees (angle between velocity and tire direction)
    /// Y-axis: Friction coefficient (typically 0-1.2)
    /// Default: realistic tire slip angle curve
    std::vector<FrictionCurvePoint> lateralFriction = {
        {0.0f,  0.0f}, // No friction when aligned
        {3.0f,  1.2f}, // Peak friction at 3 degrees
        {20.0f, 1.0f}  // Reduced friction at high slip angles
    };

    /// Maximum brake torque (Nm) that can be applied to this wheel
    float maxBrakeTorque = 1500.0f;

    /// Maximum handbrake torque (Nm) - usually only applied to rear wheels
    float maxHandBrakeTorque = 4000.0f;

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
