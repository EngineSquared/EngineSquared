#include "PluginInput.hpp"

#include "InputManager.hpp"

void ES::Plugin::Input::Plugin::Bind()
{
    RegisterResource<ES::Plugin::Input::Resource::InputManager>(ES::Plugin::Input::Resource::InputManager());
}
