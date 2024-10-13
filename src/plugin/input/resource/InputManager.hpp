#pragma once

#include <GLFW/glfw3.h>

#include <functional>
#include <map>
#include <utility>

#include "Registry.hpp"
#include "plugin/window/resource/Window.hpp"

namespace ES::Plugin::Input::Resource
{
    /**
     * InputManager is a singleton class that wraps the GLFW input functions.
     */
    class InputManager
    {
    public:
        InputManager() = default;
        ~InputManager() = default;

        inline bool IsKeyPressed(int key)
        {
            return glfwGetKey(glfwGetCurrentContext(), key) == GLFW_PRESS;
        }

        inline bool IsMouseButtonPressed(int button)
        {
            return glfwGetMouseButton(glfwGetCurrentContext(), button) == GLFW_PRESS;
        }

        inline std::pair<double, double> GetMousePosition()
        {
            double x, y;
            glfwGetCursorPos(glfwGetCurrentContext(), &x, &y);
            return {x, y};
        }
    };
}