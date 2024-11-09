#pragma once

#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

#include "Registry.hpp"

namespace ES::Plugin::Input::Resource {
/**
 * InputManager is a singleton class that wraps the GLFW input functions.
 */
class InputManager {
  public:
    InputManager() = default;
    ~InputManager() = default;

    /**
     * @brief Check if a key is pressed.
     *
     * @param key The key to check. (Use GLFW_KEY_* constants)
     * @return True if the key is pressed, false otherwise.
     */
    inline bool IsKeyPressed(int key) { return glfwGetKey(glfwGetCurrentContext(), key) == GLFW_PRESS; }

    /**
     * @brief Check if a mouse button is pressed.
     *
     * @param button The mouse button to check. (Use GLFW_MOUSE_BUTTON_* constants)
     * @return True if the mouse button is pressed, false otherwise.
     */
    inline bool IsMouseButtonPressed(int button)
    {
        return glfwGetMouseButton(glfwGetCurrentContext(), button) == GLFW_PRESS;
    }

    /**
     * @brief Get the current mouse position.
     *
     * @return A pair containing the x and y coordinates of the mouse.
     */
    inline glm::vec2 GetMousePosition()
    {
        double x, y;
        glfwGetCursorPos(glfwGetCurrentContext(), &x, &y);
        return {x, y};
    }
};
} // namespace ES::Plugin::Input::Resource
