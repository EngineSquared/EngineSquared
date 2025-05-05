#pragma once

#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

#include "Core.hpp"
#include "InputError.hpp"
#include "Logger.hpp"

namespace ES::Plugin::Input::Utils {
using JoystickAxes = std::vector<float>;
using JoystickButtons = std::vector<unsigned char>;

/**
 * @brief Check if a key is pressed.
 *
 * @param key The key to check. (Use GLFW_KEY_* constants)
 * @return True if the key is pressed, false otherwise.
 */
inline bool IsKeyPressed(int key) noexcept { return glfwGetKey(glfwGetCurrentContext(), key) == GLFW_PRESS; }

/**
 * @brief Check if a mouse button is pressed.
 *
 * @param button The mouse button to check. (Use GLFW_MOUSE_BUTTON_* constants)
 * @return True if the mouse button is pressed, false otherwise.
 */
inline bool IsMouseButtonPressed(int button) noexcept
{
    return glfwGetMouseButton(glfwGetCurrentContext(), button) == GLFW_PRESS;
}

/**
 * @brief Get the current mouse position.
 *
 * @return A pair containing the x and y coordinates of the mouse.
 */
inline glm::vec2 GetMousePosition() noexcept
{
    double x;
    double y;
    glfwGetCursorPos(glfwGetCurrentContext(), &x, &y);
    return {x, y};
}

/**
 * @brief Prints the available controllers that glfw can handle.
 */
void PrintAvailableControllers() noexcept;

/**
 * @brief Check if a joystick is present.
 *
 * @param jid The joystick ID (GLFW_JOYSTICK_1, GLFW_JOYSTICK_2, etc.)
 * @return True if the joystick is present, false otherwise.
 */
inline bool IsJoystickPresent(int jid) noexcept { return glfwJoystickPresent(jid); }

/**
 * @brief Get the name of a joystick.
 *
 * @param jid The joystick ID (GLFW_JOYSTICK_1, GLFW_JOYSTICK_2, etc.)
 * @return The name of the joystick.
 */
inline std::string GetJoystickName(int jid) noexcept
{
    if (IsJoystickPresent(jid))
    {
        return std::string(glfwGetJoystickName(jid));
    }
    return "Unknown";
}

/**
 * @brief Get the joystick axis values.
 *
 * @param jid The joystick ID (GLFW_JOYSTICK_1, GLFW_JOYSTICK_2, etc.)
 * @return A vector of axis values.
 * @throw InputError if the joystick is not present or if there is an error retrieving the axes.
 */
JoystickAxes GetJoystickAxes(int jid);

/**
 * @brief Get the joystick button states.
 *
 * @param jid The joystick ID (GLFW_JOYSTICK_1, GLFW_JOYSTICK_2, etc.)
 * @return A vector of button states.
 * @throw InputError if the joystick is not present or if there is an error retrieving the buttons.
 */
JoystickButtons GetJoystickButtons(int jid);
} // namespace ES::Plugin::Input::Utils
