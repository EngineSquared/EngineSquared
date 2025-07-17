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
    glfwWindowHint(GLFW_SAMPLES, 4);
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // <-- extra info for glfwCreateWindow
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
}

void LinkGLFWContextToGL(ES::Engine::Core &core)
{
    glfwMakeContextCurrent(core.GetResource<Resource::Window>().GetGLFWWindow());
}

void SwapBuffers(ES::Engine::Core &core) { glfwSwapBuffers(core.GetResource<Resource::Window>().GetGLFWWindow()); }

void StopSystems(ES::Engine::Core &core)
{
    if (core.GetResource<Resource::Window>().ShouldClose())
    {
        core.Stop();
    }
}

void StoreCoreInWindow(ES::Engine::Core &core)
{
    glfwSetWindowUserPointer(core.GetResource<Resource::Window>().GetGLFWWindow(), &core);
}

void DestroyWindow(ES::Engine::Core &core)
{
    core.GetResource<Resource::Window>().Destroy();
    glfwTerminate();
}

} // namespace ES::Plugin::Window::System
