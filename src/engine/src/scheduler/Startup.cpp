#include "Engine.pch.hpp"

#include "Startup.hpp"

void ES::Engine::Scheduler::Startup::RunSystems()
{
    for (auto const &system : this->GetSystems())
    {
        (*system)(_core);
    }

    _callback();
}
