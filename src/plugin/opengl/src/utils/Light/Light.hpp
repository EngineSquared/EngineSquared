#pragma once

#include <glm/glm.hpp>

namespace ES::Plugin::OpenGL::Utils {
struct Light {
    glm::vec4 position;
    glm::vec3 intensity;
};
} // namespace ES::Plugin::OpenGL::Utils
