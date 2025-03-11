#pragma once

#include <map>
#include <glm/glm.hpp>
#include "GLFW/glfw3.h"

namespace ESGL {
    struct Button
    {
        bool pressed = false;
        bool updated = false;
    };

    struct Buttons
    {
        std::map<int, Button> mouse = {
            {GLFW_MOUSE_BUTTON_LEFT, Button()},
            {GLFW_MOUSE_BUTTON_RIGHT, Button()},
            {GLFW_MOUSE_BUTTON_MIDDLE, Button()}
        };
        glm::vec<2, double, glm::defaultp> lastMousePos = {0.0f, 0.0f};
        glm::vec<2, double, glm::defaultp> currentMousePos = {0.0f, 0.0f};
    };
}