#pragma once

#include <glm/gtc/quaternion.hpp>
#include <glm/vec2.hpp>

namespace ES::Plugin::Math {
/**
 * @brief A struct representing a rectangle.
 *
 * @param   position    The position of the upper left corner of the rectangle.
 * @param   size        The size of the rectangle.
 */
struct Rect {
    glm::vec2 position;
    glm::vec2 size;
};

glm::vec3 QuaternionToEuler(const glm::quat &q);
}; // namespace ES::Plugin::Math
