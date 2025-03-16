#include "Update.hpp"

void ES::Engine::Scheduler::Update::RunSystems(USystemList &systems)
{
    for (auto const &system : systems)
    {
        (*system)(_registry);
    }
}
