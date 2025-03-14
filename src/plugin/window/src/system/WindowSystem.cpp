#include "WindowSystem.hpp"

namespace ES::Plugin::Window::System {

void CreateWindow(ES::Engine::Core &core)
{
    try
    {
        core.RegisterResource<Resource::Window>(
            Resource::Window(DEFAULT_WIDTH, DEFAULT_HEIGHT, "EngineSquarred", nullptr, nullptr));
    }
    catch (const Exception::WindowError &e)
    {
        std::cerr << e.what() << std::endl;
    }
}

void EnableVSync(ES::Engine::Core &core) { glfwSwapInterval(1); }

void PollEvents(ES::Engine::Core &core) { glfwPollEvents(); }

void UpdateKey(ES::Engine::Core &core)
{
    GLFWwindow *window = core.GetResource<Resource::Window>().GetGLFWWindow();
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

} // namespace ES::Plugin::Window::System
