#include "Input.pch.hpp"

#include "BindCallbacksToGLFW.hpp"
#include "InputManager.hpp"
#include "PluginInput.hpp"

void ES::Plugin::Input::Plugin::Bind()
{
    RequirePlugins<ES::Plugin::Window::Plugin>();

    RegisterResource<ES::Plugin::Input::Resource::InputManager>(ES::Plugin::Input::Resource::InputManager());

    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::Input::System::BindCallbacksToGLFW);
}
