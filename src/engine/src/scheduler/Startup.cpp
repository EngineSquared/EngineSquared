#include "Startup.hpp"

void ES::Engine::Scheduler::Startup::RunSystems()
{
    for (auto const &system : this->GetSystems())
    {
        RunSystem(system.get(), _core);
    }

    _callback();
}
