#include "Startup.hpp"

void ES::Engine::Scheduler::Startup::operator()(std::vector<USystem> systems)
{
    if (!_firstRun)
    {
        return;
    }

    for (auto &system : systems)
    {
        system(_registry);
    }

    _firstRun = false;
}
