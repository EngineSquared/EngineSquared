#pragma once

namespace Physics::Component {

/**
 * @brief Vehicle input controller component
 *
 * Attach this component to a vehicle chassis entity to control it.
 * Set input values between -1.0 and 1.0, which will be applied
 * to the Jolt vehicle controller during the physics update.
 */
struct VehicleController {
    /// Forward/backward input (-1.0 = full brake/reverse, 0.0 = neutral, 1.0 = full throttle)
    float forwardInput = 0.0f;

    /// Steering input (-1.0 = full left, 0.0 = straight, 1.0 = full right)
    float steeringInput = 0.0f;

    /// Brake input (0.0 = no brake, 1.0 = full brake)
    float brakeInput = 0.0f;

    /// Handbrake input (0.0 = off, 1.0 = full handbrake)
    float handBrakeInput = 0.0f;

    /**
     * @brief Set forward/backward input
     * @param value Input value clamped to [-1.0, 1.0]
     */
    void SetForward(float value)
    {
        forwardInput = std::clamp(value, -1.0f, 1.0f);
    }

    /**
     * @brief Set steering input
     * @param value Input value clamped to [-1.0, 1.0]
     */
    void SetSteering(float value)
    {
        steeringInput = std::clamp(value, -1.0f, 1.0f);
    }

    /**
     * @brief Set brake input
     * @param value Input value clamped to [0.0, 1.0]
     */
    void SetBrake(float value)
    {
        brakeInput = std::clamp(value, 0.0f, 1.0f);
    }

    /**
     * @brief Set handbrake input
     * @param value Input value clamped to [0.0, 1.0]
     */
    void SetHandBrake(float value)
    {
        handBrakeInput = std::clamp(value, 0.0f, 1.0f);
    }

    /**
     * @brief Reset all inputs to neutral
     */
    void ResetInputs()
    {
        forwardInput = 0.0f;
        steeringInput = 0.0f;
        brakeInput = 0.0f;
        handBrakeInput = 0.0f;
    }
};

} // namespace Physics::Component
