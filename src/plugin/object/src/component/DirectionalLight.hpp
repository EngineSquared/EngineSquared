#pragma once

#include <glm/glm.hpp>

namespace Object::Component {

struct DirectionalLight {
    glm::vec4 color{1.0f};
    glm::vec3 direction{0.0f, -1.0f, 0.0f};
};
} // namespace Object::Component
