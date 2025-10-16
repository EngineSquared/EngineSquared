#include "RenderingPipeline.hpp"

#include "plugin/PluginWindow.hpp"
#include "resource/Window.hpp"
#include "system/WindowSystem.hpp"

void Window::Plugin::Bind()
{
    RequirePlugins<RenderingPipeline::Plugin>();

    RegisterSystems<RenderingPipeline::Init>(Window::System::InitGLFW, Window::System::SetupGLFWHints,
                                             Window::System::CreateWindowSystem, Window::System::LinkGLFWContextToGL,
                                             Window::System::StoreCoreInWindow);

    RegisterSystems<RenderingPipeline::Setup>(Window::System::EnableVSync);

    RegisterSystems<RenderingPipeline::PreUpdate>(Window::System::PollEvents);

    RegisterSystems<Engine::Scheduler::Update>(Window::System::StopSystems);

    RegisterSystems<RenderingPipeline::Draw>(Window::System::SwapBuffers);

    RegisterSystems<Engine::Scheduler::Shutdown>(Window::System::DestroyWindow);
}
