#include "PluginInput.hpp"

#include "Startup.hpp"
#include "InputManager.hpp"

void ES::Plugin::Input::Plugin::Bind()
{
    RegisterSystems<ES::Engine::Scheduler::Startup>([](ES::Engine::Core &core) {
        core.RegisterResource<ES::Plugin::Input::Resource::InputManager>(ES::Plugin::Input::Resource::InputManager());
    });
}
