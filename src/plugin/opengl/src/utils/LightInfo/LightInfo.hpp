#pragma once

#include <glm/gtc/type_ptr.hpp>

namespace ES::Plugin::OpenGL::Utils {

struct alignas(16) LightInfo {
    glm::vec4 position;  // .xyz = position, .w = type
    glm::vec4 intensity; // .xyz = intensité, .w = unused
};

} // namespace ES::Plugin::OpenGL::Utils
