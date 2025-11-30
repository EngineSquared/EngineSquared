#include "plugin/PluginEvent.hpp"
#include "resource/EventManager.hpp"
#include "scheduler/Update.hpp"
#include "system/EventSystem.hpp"

void Event::Plugin::Bind()
{
    RegisterResource<Resource::EventManager>(Resource::EventManager());
    RegisterSystems<Engine::Scheduler::Update>(System::ProcessEvents);
}
