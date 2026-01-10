#include "WindowSystem.hpp"
#include "Logger.hpp"
#include "resource/EventManager.hpp"

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

void SetupGLFWHints(const Engine::Core &) { glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); }

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

void SetupWindowCallbacks(Engine::Core &core)
{
    GLFWwindow *glfwWindow = core.GetResource<Resource::Window>().GetGLFWWindow();

    glfwSetFramebufferSizeCallback(
        glfwWindow,
        [](GLFWwindow *window, int width, int height) {
            Engine::Core *core = static_cast<Engine::Core *>(glfwGetWindowUserPointer(window));
            auto &eventManager = core->GetResource<::Event::Resource::EventManager>();
            eventManager.PushEvent(Window::Event::OnResize{glm::ivec2{width, height}});
        });
}

} // namespace Window::System
