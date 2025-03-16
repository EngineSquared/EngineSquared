#include "Shutdown.hpp"

void ES::Engine::Scheduler::Shutdown::RunSystems(USystemList &systems)
{
    for (auto &system : systems)
    {
        (*system)(_registry);
    }

    _callback();
}
