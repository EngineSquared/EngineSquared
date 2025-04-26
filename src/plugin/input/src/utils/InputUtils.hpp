#pragma once

#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

#include "Core.hpp"

#include "Logger.hpp"

namespace ES::Plugin::Input::Utils {
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
    double x;
    double y;
    glfwGetCursorPos(glfwGetCurrentContext(), &x, &y);
    return {x, y};
}

/**
 * @brief Prints the available controllers that glfw can handle.
 */
inline void PrintAvailableControllers()
{
    for (int jid = GLFW_JOYSTICK_1; jid <= GLFW_JOYSTICK_LAST; ++jid)
    {
        if (glfwJoystickPresent(jid))
        {
            const char *name = glfwGetJoystickName(jid);
            ES::Utils::Log::Info(fmt::format("Detected controller {}: {}", jid, name ? name : "Unknown"));
        }
    }
}
} // namespace ES::Plugin::Input::Utils
