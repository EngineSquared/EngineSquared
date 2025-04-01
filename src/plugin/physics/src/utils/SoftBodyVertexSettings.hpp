#pragma once

#include <glm/glm.hpp>

namespace ES::Plugin::Physics::Utils {
struct SoftBodyVertexSettings {
    /// @brief Initial velocity of each vertex.
    /// @note This will only be used when the soft body is created.
    /// @note The same velocity will be applied to all vertices.
    glm::vec3 initialVelocity = glm::vec3(0.0f);

    /// @brief Inverse mass of each vertex.
    /// @note This will only be used when the soft body is created.
    /// @note The same inverse mass will be applied to all vertices.
    float invMass = 1.0f;
};
} // namespace ES::Plugin::Physics::Utils
