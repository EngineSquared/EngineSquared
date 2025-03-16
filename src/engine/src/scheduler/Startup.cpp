#include "Startup.hpp"

void ES::Engine::Scheduler::Startup::RunSystems(USystemList &systems)
{
    for (auto const &system : systems)
    {
        (*system)(_registry);
    }

    _callback();
}
