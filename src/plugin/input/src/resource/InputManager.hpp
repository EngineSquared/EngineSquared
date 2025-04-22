#pragma once

#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

#include "Core.hpp"

namespace ES::Plugin::Input::Resource {
/**
 * InputManager is a singleton class that wraps the GLFW callback functions.
 */
class InputManager {
  public:
    InputManager();
    ~InputManager() = default;

    /**
     * @brief Print the available controllers detected by glfw.
     *
     * @return void
    */
    void PrintAvailableControllers() const;

  private:
    /**
     * @brief GLFW callback function to handle key events.
     *
     * @param window The window that received the event.
     * @param key The key that was pressed or released.
     * @param scancode The scancode of the key.
     * @param action The action that was performed (pressed, released, repeated).
     * @param mods The modifier keys that were held down during the event.
     */
    static void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

    /**
     * @brief GLFW callback function to handle character input events.
     *
     * @param window The window that received the event.
     * @param codepoint The Unicode code point of the character.
     */
    static void CharCallback(GLFWwindow *window, unsigned int codepoint);

    /**
     * @brief GLFW callback function to handle char mods input events.
     *
     * @param window The window that received the event.
     * @param codepoint The Unicode code point of the character.
     * @param mods The modifier keys that were held down during the event.
     */
    static void CharModsCallback(GLFWwindow *window, unsigned int codepoint, int mods);

    /**
     * @brief GLFW callback function to handle mouse button events.
     *
     * @param window The window that received the event.
     * @param button The mouse button that was pressed or released.
     * @param action The action that was performed (pressed, released).
     * @param mods The modifier keys that were held down during the event.
     */
    static void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods);

    /**
     * @brief GLFW callback function to handle mouse cursor position events.
     *
     * @param window The window that received the event.
     * @param xpos The x-coordinate of the cursor position.
     * @param ypos The y-coordinate of the cursor position.
     */
    static void CursorPosCallback(GLFWwindow *window, double xpos, double ypos);

    /**
     * @brief GLFW callback function to handle mouse cursor enter events.
     *
     * @param window The window that received the event.
     * @param entered True if the cursor entered the window, false if it left.
     */
    static void CursorEnterCallback(GLFWwindow *window, int entered);

    /**
     * @brief GLFW callback function to handle mouse scroll events.
     *
     * @param window The window that received the event.
     * @param xoffset The x-offset of the scroll wheel.
     * @param yoffset The y-offset of the scroll wheel.
     */
    static void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset);

    /**
     * @brief GLFW callback function to handle file drop events.
     *
     * @param window The window that received the event.
     * @param count The number of dropped files.
     * @param names The names of the dropped files.
     */
    static void DropCallback(GLFWwindow *window, int count, const char **names);

    /**
     * @brief GLFW callback function to handle joystick connection events.
     *
     * @param jid The joystick ID.
     * @param event The event type (connected or disconnected).
     */
    static void JoystickCallback(int jid, int event);
};
} // namespace ES::Plugin::Input::Resource
