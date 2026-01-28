#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Object::Component {

struct DirectionalLight {
    glm::vec4 color{1.0f};
    glm::mat4 projection = glm::orthoLH_ZO(-30.0f, 30.0f, -30.0f, 30.0f, 0.1f, 60.0f);
};
} // namespace Object::Component
