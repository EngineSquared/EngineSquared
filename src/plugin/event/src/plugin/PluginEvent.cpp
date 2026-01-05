#include "plugin/PluginEvent.hpp"
#include "resource/EventManager.hpp"
#include "scheduler/Update.hpp"
#include "system/EventSystem.hpp"

#include "scheduler/FixedTimeUpdate.hpp"
#include "scheduler/RelativeTimeUpdate.hpp"
#include "scheduler/Shutdown.hpp"
#include "scheduler/Startup.hpp"

void Event::Plugin::Bind()
{
    RegisterResource<Resource::EventManager>(Resource::EventManager());
    RegisterSystems<Engine::Scheduler::Startup>(System::ProcessEvents<Engine::Scheduler::Startup>);
    RegisterSystems<Engine::Scheduler::Update>(System::ProcessEvents<Engine::Scheduler::Update>);
    RegisterSystems<Engine::Scheduler::FixedTimeUpdate>(System::ProcessEvents<Engine::Scheduler::FixedTimeUpdate>);
    RegisterSystems<Engine::Scheduler::RelativeTimeUpdate>(
        System::ProcessEvents<Engine::Scheduler::RelativeTimeUpdate>);
    RegisterSystems<Engine::Scheduler::Shutdown>(System::ProcessEvents<Engine::Scheduler::Shutdown>);
}
