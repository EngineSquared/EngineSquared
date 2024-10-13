#pragma once

#include <functional>
#include <map>

#include <GLFW/glfw3.h>

#include "plugin/window/resource/Window.hpp"


namespace ES::Plugin::Input::Utils {
    /**
     * CursorCallback is a simple struct that holds a callback function for cursor position events.
     */
    struct CursorCallback
    {
        int key;
        std::function<void(GLFWwindow*, double, double)> callback;
    };
}