#include "PluginWindow.hpp"
#include "Startup.hpp"
#include "system/WindowSystem.hpp"

void ES::Plugin::Window::Plugin::Bind()
{
    RegisterResource<ES::Plugin::Window::Resource::Buttons>({});

    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::Window::System::InitGLFW);
    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::Window::System::SetupGLFWHints);
    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::Window::System::LinkGLFWContextToGL);
    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::Window::System::EnableVSync);
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::Window::System::UpdateKey);
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::Window::System::UpdatePosCursor);
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::Window::System::UpdateButton);
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::Window::System::SaveLastMousePos);
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::Window::System::SwapBuffers);
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::Window::System::PollEvents);
    // RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::Window::System::MouseDragging);
}
