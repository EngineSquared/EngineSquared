#pragma once

#include <glm/glm.hpp>

namespace Physics::Component {
/// @brief Component representing a character controller, which allows for easy character movement and collision
///     handling.
struct CharacterController {
    /// @brief Maximum slope angle (in degrees) that the character can walk up
    float maxSlopeAngle = 50.0f;
    /// @brief Maximum height of steps that the character can climb
    float maxStepHeight = 0.3f;
    /// @brief Factor controlling how much gravity affects the character (1.0 = normal gravity, 0.0 = no gravity)
    float gravityFactor = 1.0f;
    /// @brief Mass of the character (used for physics interactions)
    float mass = 70.0f;
    /// @brief Current linear velocity of the character (in world space)
    glm::vec3 linearVelocity{0.0f, 0.0f, 0.0f};
};

} // namespace Physics::Component
