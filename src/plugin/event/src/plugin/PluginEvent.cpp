#include "plugin/PluginEvent.hpp"
#include "resource/EventManager.hpp"
#include "scheduler/Update.hpp"
#include "system/EventSystem.hpp"

#include "scheduler/FixedTimeUpdate.hpp"
#include "scheduler/RelativeTimeUpdate.hpp"

void Event::Plugin::Bind()
{
    RegisterResource<Resource::EventManager>(Resource::EventManager());
    RegisterSystems<Engine::Scheduler::Update>(System::ProcessEvents<Engine::Scheduler::Update>);
    RegisterSystems<Engine::Scheduler::FixedTimeUpdate>(System::ProcessEvents<Engine::Scheduler::FixedTimeUpdate>);
    RegisterSystems<Engine::Scheduler::RelativeTimeUpdate>(
        System::ProcessEvents<Engine::Scheduler::RelativeTimeUpdate>);
}
