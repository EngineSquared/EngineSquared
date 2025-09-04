#include "Engine.pch.hpp"

#include "core/Core.hpp"
#include "scheduler/Shutdown.hpp"

void ES::Engine::Scheduler::Shutdown::RunSystems()
{
    if (_core.IsRunning())
    {
        return;
    }
    for (auto const &system : this->GetSystems())
    {
        RunSystem(system.get(), _core);
    }
}
