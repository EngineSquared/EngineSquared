#include "Engine.pch.hpp"

#include "scheduler/Startup.hpp"

Engine::Scheduler::Startup::Startup(Core &core) : AScheduler(core) {}

void Engine::Scheduler::Startup::RunSystems()
{
    std::set<FunctionUtils::FunctionID> systemIdsToDisable{};
    for (auto const &system : this->GetSystems())
    {
        RunSystem(system.get(), _core);
        systemIdsToDisable.insert(system.get()->GetID());
    }

    for (auto const &systemIdToDisable : systemIdsToDisable)
    {
        this->Disable(systemIdToDisable);
    }
}
