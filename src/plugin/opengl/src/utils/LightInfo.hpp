#pragma once

#include <glm/gtc/type_ptr.hpp>

namespace ES::Plugin::OpenGL::Utils {

struct LightInfo {
    glm::vec4 position; // .xyz = position, .w = type
    glm::vec4 colour;   // .rgb = colour, .a = intensity
};

} // namespace ES::Plugin::OpenGL::Utils
