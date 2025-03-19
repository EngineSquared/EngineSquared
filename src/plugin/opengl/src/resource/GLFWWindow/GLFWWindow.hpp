#pragma once

#ifndef __glew_h__
// #    define GLEW_STATIC
#    include <GL/glew.h>
#endif
#include "GLFW/glfw3.h"

namespace ES::Plugin::OpenGL::Resource {
struct GLFWWindow {
    GLFWwindow *window;
};
} // namespace ES::Plugin::OpenGL::Resource
