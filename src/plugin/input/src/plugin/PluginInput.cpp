#include "Input.pch.hpp"

#include "plugin/PluginInput.hpp"
#include "resource/InputManager.hpp"
#include "system/BindCallbacksToGLFW.hpp"

void Plugin::Input::Plugin::Bind()
{
    RequirePlugins<Window::Plugin>();

    RegisterResource<Resource::InputManager>(Resource::InputManager(GetCore()));

    RegisterSystems<Engine::Scheduler::Startup>(System::BindCallbacksToGLFW);
}
