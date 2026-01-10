#pragma once

#include <glm/glm.hpp>

namespace Object::Component {

struct AmbientLight {
    glm::vec3 color = glm::vec3(1.0f);
};
} // namespace Object::Component
