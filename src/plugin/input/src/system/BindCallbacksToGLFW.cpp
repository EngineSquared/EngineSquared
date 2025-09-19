#include "Input.pch.hpp"

#include "Input.hpp"
#include "resource/Window.hpp"
#include "system/BindCallbacksToGLFW.hpp"

/**
 * @brief Get the core from the window user pointer.
 *
 * @return A reference to the core.
 *
 * @note This is needed as the callbacks need to access the core, but they
 * also need to be static functions to be used as GLFW callbacks.
 */
static Engine::Core &GetCoreFromWindow(GLFWwindow *window)
{
    return *static_cast<Engine::Core *>(glfwGetWindowUserPointer(window));
}

static void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    Engine::Core &core = GetCoreFromWindow(window);
    auto &inputManager = core.GetResource<Plugin::Input::Resource::InputManager>();
    inputManager.CallKeyCallbacks(core, key, scancode, action, mods);
}

static void CharCallback(GLFWwindow *window, unsigned int codepoint)
{
    Engine::Core &core = GetCoreFromWindow(window);
    auto &inputManager = core.GetResource<Plugin::Input::Resource::InputManager>();
    inputManager.CallCharCallbacks(core, codepoint);
}

static void CharModsCallback(GLFWwindow *window, unsigned int codepoint, int mods)
{
    Engine::Core &core = GetCoreFromWindow(window);
    auto &inputManager = core.GetResource<Plugin::Input::Resource::InputManager>();
    inputManager.CallCharModsCallbacks(core, codepoint, mods);
}

static void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    Engine::Core &core = GetCoreFromWindow(window);
    auto &inputManager = core.GetResource<Plugin::Input::Resource::InputManager>();
    inputManager.CallMouseButtonCallbacks(core, button, action, mods);
}

static void CursorPosCallback(GLFWwindow *window, double xpos, double ypos)
{
    Engine::Core &core = GetCoreFromWindow(window);
    auto &inputManager = core.GetResource<Plugin::Input::Resource::InputManager>();
    inputManager.CallCursorPosCallbacks(core, xpos, ypos);
}

static void CursorEnterCallback(GLFWwindow *window, int entered)
{
    Engine::Core &core = GetCoreFromWindow(window);
    auto &inputManager = core.GetResource<Plugin::Input::Resource::InputManager>();
    inputManager.CallCursorEnterCallbacks(core, entered);
}

static void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
    Engine::Core &core = GetCoreFromWindow(window);
    auto &inputManager = core.GetResource<Plugin::Input::Resource::InputManager>();
    inputManager.CallScrollCallbacks(core, xoffset, yoffset);
}

static void DropCallback(GLFWwindow *window, int count, const char **paths)
{
    Engine::Core &core = GetCoreFromWindow(window);
    auto &inputManager = core.GetResource<Plugin::Input::Resource::InputManager>();
    inputManager.CallDropCallbacks(core, count, paths);
}

void Plugin::Input::System::BindCallbacksToGLFW(Engine::Core &core)
{
    auto &window = core.GetResource<Window::Resource::Window>();

    glfwSetKeyCallback(window.GetGLFWWindow(), KeyCallback);
    glfwSetCharCallback(window.GetGLFWWindow(), CharCallback);
    glfwSetCharModsCallback(window.GetGLFWWindow(), CharModsCallback);
    glfwSetMouseButtonCallback(window.GetGLFWWindow(), MouseButtonCallback);
    glfwSetCursorPosCallback(window.GetGLFWWindow(), CursorPosCallback);
    glfwSetCursorEnterCallback(window.GetGLFWWindow(), CursorEnterCallback);
    glfwSetScrollCallback(window.GetGLFWWindow(), ScrollCallback);
    glfwSetDropCallback(window.GetGLFWWindow(), DropCallback);
}
