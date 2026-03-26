#pragma once

#include <glm/glm.hpp>

namespace Physics::Component {
struct CharacterController {
    float maxSlopeAngle = 50.0f;
    float maxStepHeight = 0.3f;
    float gravityFactor = 1.0f;
    float mass = 70.0f;
    glm::vec3 linearVelocity{0.0f, 0.0f, 0.0f};
};

} // namespace Physics::Component
