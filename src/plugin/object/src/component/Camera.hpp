#pragma once

#include "glm/glm.hpp"

namespace Object::Component {

struct Camera {
    using ID = entt::hashed_string;

    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    float fov = glm::radians(70.0f);
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
};
} // namespace Object::Component
