#include "Startup.hpp"

void ES::Engine::Scheduler::Startup::RunSystems()
{
    for (auto const &system : this->GetSystems())
    {
        RunSystem(system, _core);
    }

    _callback();
}
