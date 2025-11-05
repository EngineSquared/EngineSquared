#include "WindowSystem.hpp"
#include "Logger.hpp"

namespace Window::System {

void CreateWindowSystem(Engine::Core &core)
{
    core.RegisterResource<Resource::Window>(
        Resource::Window(DEFAULT_WIDTH, DEFAULT_HEIGHT, "EngineSquared", nullptr, nullptr));
}

void EnableVSync(Engine::Core &) { glfwSwapInterval(1); }

void PollEvents(Engine::Core &) { glfwPollEvents(); }

void InitGLFW(const Engine::Core &)
{
    if (!glfwInit())
    {
        Log::Error("Failed to initialize GLFW");
        return;
    }
}

void SetupGLFWHints(const Engine::Core &)
{
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void LinkGLFWContextToGL(Engine::Core &core)
{
    glfwMakeContextCurrent(core.GetResource<Resource::Window>().GetGLFWWindow());
}

void SwapBuffers(Engine::Core &core) { glfwSwapBuffers(core.GetResource<Resource::Window>().GetGLFWWindow()); }

void StopSystems(Engine::Core &core)
{
    if (core.GetResource<Resource::Window>().ShouldClose())
    {
        core.Stop();
    }
}

void StoreCoreInWindow(Engine::Core &core)
{
    glfwSetWindowUserPointer(core.GetResource<Resource::Window>().GetGLFWWindow(), &core);
}

void DestroyWindow(Engine::Core &core)
{
    core.GetResource<Resource::Window>().Destroy();
    glfwTerminate();
}

} // namespace Window::System
