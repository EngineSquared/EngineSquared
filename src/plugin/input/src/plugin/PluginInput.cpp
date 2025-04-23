#include "BindCallbacksToGLFW.hpp"
#include "PluginInput.hpp"
#include "InputManager.hpp"
#include "PluginWindow.hpp"
#include "Startup.hpp"
#include "Window.hpp"

void ES::Plugin::Input::Plugin::Bind()
{
    RequirePlugins<ES::Plugin::Window::Plugin>();

    RegisterResource<ES::Plugin::Input::Resource::InputManager>(ES::Plugin::Input::Resource::InputManager());

    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::Input::System::BindCallbacksToGLFW);
}
