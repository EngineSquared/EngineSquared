#include "PluginWindow.hpp"
#include "WindowSystem.hpp"
#include "RenderingPipeline.hpp"

void ES::Plugin::Window::Plugin::Bind()
{
    RequirePlugins<ES::Plugin::RenderingPipeline::Plugin>();

    RegisterSystems<ES::Plugin::RenderingPipeline::Init>(
        ES::Plugin::Window::System::InitGLFW,
        ES::Plugin::Window::System::SetupGLFWHints,
        ES::Plugin::Window::System::CreateWindowSystem,
        ES::Plugin::Window::System::LinkGLFWContextToGL);

    RegisterSystems<ES::Plugin::RenderingPipeline::Setup>(
        ES::Plugin::Window::System::EnableVSync,
        ES::Plugin::Window::System::LoadButtons);

    RegisterSystems<ES::Plugin::RenderingPipeline::PreUpdate>(
        ES::Plugin::Window::System::PollEvents,
        ES::Plugin::Window::System::UpdateKey,
        ES::Plugin::Window::System::UpdatePosCursor,
        ES::Plugin::Window::System::UpdateButton,
        ES::Plugin::Window::System::SaveLastMousePos);

    RegisterSystems<ES::Engine::Scheduler::Update>(
        ES::Plugin::Window::System::StopSystems);

    RegisterSystems<ES::Plugin::RenderingPipeline::Draw>(
        ES::Plugin::Window::System::SwapBuffers);
}
