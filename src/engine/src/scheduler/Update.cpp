#include "Update.hpp"

void ES::Engine::Scheduler::Update::RunSystems(std::vector<std::unique_ptr<SystemBase>> &systems)
{
    for (auto const &system : systems)
    {
        (*system)(_registry);
    }
}
