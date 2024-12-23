#include "Startup.hpp"

void ES::Engine::Scheduler::Startup::RunSystems(std::vector<USystem> systems)
{
    for (auto &system : systems)
    {
        system(_registry);
    }

    _callback();
}
