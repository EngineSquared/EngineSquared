#include "Update.hpp"

void ES::Engine::Scheduler::Update::operator()(std::vector<USystem> systems)
{
    for (auto &system : systems)
    {
        system(_registry);
    }
}