#include "Engine.pch.hpp"

#include "scheduler/Startup.hpp"

Engine::Scheduler::Startup::Startup(Core &core, const std::function<void()> &callback)
    : AScheduler(core), _callback(callback)
{
}

void Engine::Scheduler::Startup::RunSystems()
{
    for (auto const &system : this->GetSystems())
    {
        RunSystem(system.get(), _core);
    }

    _callback();
}
