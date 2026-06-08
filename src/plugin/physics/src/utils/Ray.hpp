#pragma once

#include <glm/vec3.hpp>

namespace Physics::Utils {
struct Ray {
    glm::vec3 origin;
    glm::vec3 direction;
};
} // namespace Physics::Utils
