#include "InputManager.hpp"

#include <fmt/format.h>
#include <GLFW/glfw3.h>

#include "Core.hpp"
#include "Logger.hpp"

ES::Plugin::Input::Resource::InputManager::InputManager()
{
    PrintAvailableControllers(); // TODO: enclose in ES_DEBUG

    glfwSetKeyCallback(glfwGetCurrentContext(), KeyCallback);
    glfwSetCharCallback(glfwGetCurrentContext(), CharCallback);
    glfwSetCharModsCallback(glfwGetCurrentContext(), CharModsCallback);
    glfwSetMouseButtonCallback(glfwGetCurrentContext(), MouseButtonCallback);
    glfwSetCursorPosCallback(glfwGetCurrentContext(), CursorPosCallback);
    glfwSetCursorEnterCallback(glfwGetCurrentContext(), CursorEnterCallback);
    glfwSetScrollCallback(glfwGetCurrentContext(), ScrollCallback);
    glfwSetDropCallback(glfwGetCurrentContext(), DropCallback);
    glfwSetJoystickCallback(JoystickCallback);
}

void ES::Plugin::Input::Resource::InputManager::PrintAvailableControllers() const
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

void ES::Plugin::Input::Resource::InputManager::KeyCallback(GLFWwindow *window, int key, int scancode, int action,
                                                            int mods)
{
    if (action == GLFW_PRESS)
    {
        ES::Utils::Log::Info(fmt::format("Key pressed: {} (scancode: {}, mods: {})", key, scancode, mods));
    }
    else if (action == GLFW_RELEASE)
    {
        ES::Utils::Log::Info(fmt::format("Key released: {} (scancode: {}, mods: {})", key, scancode, mods));
    }
}

void ES::Plugin::Input::Resource::InputManager::CharCallback(GLFWwindow *window, unsigned int codepoint)
{
    ES::Utils::Log::Info(fmt::format("Character input: {}", codepoint));
}

void ES::Plugin::Input::Resource::InputManager::CharModsCallback(GLFWwindow *window, unsigned int codepoint, int mods)
{
    ES::Utils::Log::Info(fmt::format("Character input with mods: {} (mods: {})", codepoint, mods));
}

void ES::Plugin::Input::Resource::InputManager::MouseButtonCallback(GLFWwindow *window, int button, int action,
                                                                    int mods)
{
    if (action == GLFW_PRESS)
    {
        ES::Utils::Log::Info(fmt::format("Mouse button pressed: {} (mods: {})", button, mods));
    }
    else if (action == GLFW_RELEASE)
    {
        ES::Utils::Log::Info(fmt::format("Mouse button released: {} (mods: {})", button, mods));
    }
}

void ES::Plugin::Input::Resource::InputManager::CursorPosCallback(GLFWwindow *window, double xpos, double ypos)
{
    ES::Utils::Log::Info(fmt::format("Mouse cursor position: ({}, {})", xpos, ypos));
}

void ES::Plugin::Input::Resource::InputManager::CursorEnterCallback(GLFWwindow *window, int entered)
{
    if (entered)
    {
        ES::Utils::Log::Info("Mouse cursor entered the window");
    }
    else
    {
        ES::Utils::Log::Info("Mouse cursor left the window");
    }
}

void ES::Plugin::Input::Resource::InputManager::ScrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
    ES::Utils::Log::Info(fmt::format("Mouse scroll: ({}, {})", xoffset, yoffset));
}

void ES::Plugin::Input::Resource::InputManager::DropCallback(GLFWwindow *window, int count, const char **paths)
{
    for (int i = 0; i < count; ++i)
    {
        ES::Utils::Log::Info(fmt::format("Dropped file: {}", paths[i]));
    }
}

void ES::Plugin::Input::Resource::InputManager::JoystickCallback(int jid, int event)
{
    if (event == GLFW_CONNECTED)
    {
        ES::Utils::Log::Info(fmt::format("Joystick {} connected", jid));
    }
    else if (event == GLFW_DISCONNECTED)
    {
        ES::Utils::Log::Info(fmt::format("Joystick {} disconnected", jid));
    }
}
