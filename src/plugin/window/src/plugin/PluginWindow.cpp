#include "PluginWindow.hpp"
#include "RenderingPipeline.hpp"
#include "WindowSystem.hpp"

void ES::Plugin::Window::Plugin::Bind()
{
    RequirePlugins<ES::Plugin::RenderingPipeline::Plugin>();

    RegisterSystems<ES::Plugin::RenderingPipeline::Init>(
        ES::Plugin::Window::System::InitGLFW, ES::Plugin::Window::System::SetupGLFWHints,
        ES::Plugin::Window::System::CreateWindowSystem, ES::Plugin::Window::System::LinkGLFWContextToGL,
        ES::Plugin::Window::System::StoreCoreInWindow);

    RegisterSystems<ES::Plugin::RenderingPipeline::Setup>(ES::Plugin::Window::System::EnableVSync);

    RegisterSystems<ES::Plugin::RenderingPipeline::PreUpdate>(ES::Plugin::Window::System::PollEvents);

    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::Window::System::StopSystems);

    RegisterSystems<ES::Plugin::RenderingPipeline::Draw>(ES::Plugin::Window::System::SwapBuffers);

    RegisterSystems<ES::Engine::Scheduler::Shutdown>(ES::Plugin::Window::System::DestroyWindow);
}
