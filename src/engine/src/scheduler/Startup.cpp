#include "Engine.pch.hpp"

#include "scheduler/Startup.hpp"

void Engine::Scheduler::Startup::RunSystems()
{
    for (auto const &system : this->GetSystems())
    {
        RunSystem(system.get(), _core);
    }

    _callback();
}
