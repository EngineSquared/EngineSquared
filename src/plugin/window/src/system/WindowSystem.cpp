#include "WindowSystem.hpp"
#include "Logger.hpp"

namespace ES::Plugin::Window::System {

void CreateWindowSystem(ES::Engine::Core &core)
{
    core.RegisterResource<Resource::Window>(
        Resource::Window(DEFAULT_WIDTH, DEFAULT_HEIGHT, "EngineSquared", nullptr, nullptr));
}

void EnableVSync(ES::Engine::Core &) { glfwSwapInterval(1); }

void PollEvents(ES::Engine::Core &) { glfwPollEvents(); }

void UpdateKey(ES::Engine::Core &core)
{
    GLFWwindow *window = core.GetResource<Resource::Window>().GetGLFWWindow();
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void UpdateButton(ES::Engine::Core &core)
{
    GLFWwindow *window = core.GetResource<Resource::Window>().GetGLFWWindow();
    auto &mouseButtons = core.GetResource<Resource::Buttons>().mouse;
    for (auto &[key, button] : mouseButtons)
    {
        bool pressed = glfwGetMouseButton(window, key) == GLFW_PRESS;
        button.updated = button.pressed != pressed;
        button.pressed = pressed;
    }
}

void SaveLastMousePos(ES::Engine::Core &core)
{
    auto &buttons = core.GetResource<Resource::Buttons>();
    auto &lastMousePos = buttons.lastMousePos;
    auto &mouseButtons = buttons.mouse;
    auto window = core.GetResource<Resource::Window>().GetGLFWWindow();
    if (mouseButtons[GLFW_MOUSE_BUTTON_LEFT].updated || mouseButtons[GLFW_MOUSE_BUTTON_MIDDLE].updated ||
        mouseButtons[GLFW_MOUSE_BUTTON_RIGHT].updated)
    {
        double xpos = 0;
        double ypos = 0;
        glfwGetCursorPos(window, &xpos, &ypos);
        lastMousePos.x = xpos;
        lastMousePos.y = ypos;
    }
}

void InitGLFW(const ES::Engine::Core &)
{
    if (!glfwInit())
    {
        ES::Utils::Log::Error("Failed to initialize GLFW");
        return;
    }
}

void SetupGLFWHints(const ES::Engine::Core &)
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void LinkGLFWContextToGL(ES::Engine::Core &core)
{
    glfwMakeContextCurrent(core.GetResource<Resource::Window>().GetGLFWWindow());
}

void UpdatePosCursor(ES::Engine::Core &core)
{
    auto &currentMousePos = core.GetResource<Resource::Buttons>().currentMousePos;
    glfwGetCursorPos(core.GetResource<Resource::Window>().GetGLFWWindow(), &currentMousePos.x, &currentMousePos.y);
}

void SwapBuffers(ES::Engine::Core &core) { glfwSwapBuffers(core.GetResource<Resource::Window>().GetGLFWWindow()); }

void StopSystems(ES::Engine::Core &core)
{
    if (glfwWindowShouldClose(core.GetResource<Resource::Window>().GetGLFWWindow()))
    {
        core.Stop();
    }
}

void LoadButtons(ES::Engine::Core &core) { core.RegisterResource<Resource::Buttons>(Resource::Buttons()); }

void StoreCoreInWindow(ES::Engine::Core &core)
{
    glfwSetWindowUserPointer(glfwGetCurrentContext(), &core);
}
} // namespace ES::Plugin::Window::System
