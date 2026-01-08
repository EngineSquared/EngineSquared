#pragma once

#include <glm/glm.hpp>

namespace Object::Component {

struct PointLight {
    glm::vec3 color = glm::vec3(1.0f);
    float intensity = 1.0f;
    float radius = 10.0f;
    float falloff = 1.0f;
};
} // namespace Object::Component
