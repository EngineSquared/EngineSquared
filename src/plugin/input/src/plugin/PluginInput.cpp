#include "PluginInput.hpp"

#include "InputManager.hpp"
#include "Startup.hpp"

void ES::Plugin::Input::Plugin::Bind()
{
    RegisterSystems<ES::Engine::Scheduler::Startup>([](ES::Engine::Core &core) {
        core.RegisterResource<ES::Plugin::Input::Resource::InputManager>(ES::Plugin::Input::Resource::InputManager());
    });
}
