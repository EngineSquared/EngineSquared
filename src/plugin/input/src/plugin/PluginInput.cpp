#include "PluginInput.hpp"

#include "InputManager.hpp"
#include "PluginWindow.hpp"
#include "Startup.hpp"
#include "Window.hpp"

void ES::Plugin::Input::Plugin::Bind()
{
    RequirePlugins<ES::Plugin::Window::Plugin>();

    RegisterSystems<ES::Engine::Scheduler::Startup>([](ES::Engine::Core &core) {
        glfwUser(
    });

    RegisterResource<ES::Plugin::Input::Resource::InputManager>(ES::Plugin::Input::Resource::InputManager());
}
