#include "plugin/PluginEvent.hpp"
#include "resource/EventManager.hpp"

void Event::Plugin::Bind()
{
    RegisterResource<Resource::EventManager>(Resource::EventManager());
}
