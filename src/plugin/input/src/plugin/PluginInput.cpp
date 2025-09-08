#include "Input.pch.hpp"

#include "plugin/PluginInput.hpp"
#include "resource/InputManager.hpp"
#include "system/BindCallbacksToGLFW.hpp"

void ES::Plugin::Input::Plugin::Bind()
{
    RequirePlugins<ES::Plugin::Window::Plugin>();

    RegisterResource<ES::Plugin::Input::Resource::InputManager>(ES::Plugin::Input::Resource::InputManager(GetCore()));

    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::Input::System::BindCallbacksToGLFW);
}
