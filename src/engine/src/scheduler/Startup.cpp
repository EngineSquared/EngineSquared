#include "Startup.hpp"

void ES::Engine::Scheduler::Startup::RunSystems(std::vector<std::unique_ptr<SystemBase>> &systems)
{
    for (auto const &system : systems)
    {
        (*system)(_registry);
    }

    _callback();
}
