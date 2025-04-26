#pragma once

#include <glm/glm.hpp>

namespace ES::Plugin::OpenGL::Utils {
struct MouseDragging {
    glm::vec2 lastMousePos{0.0f, 0.0f};
};
} // namespace ES::Plugin::OpenGL::Utils
