#include "Update.hpp"

void ES::Engine::Scheduler::Update::RunSystems(std::vector<std::unique_ptr<SystemBase>> &systems)
{
    for (auto &system : systems)
    {
        (*system)(_registry);
    }
}
