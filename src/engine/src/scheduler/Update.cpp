#include "Update.hpp"

void ES::Engine::Scheduler::Update::RunSystems(std::vector<USystem> systems)
{
    auto currentTime = std::chrono::high_resolution_clock::now();
    _elapsedTime += std::chrono::duration<float>(currentTime - _lastTime).count();
    
    for (auto &system : systems)
    {
        system(_registry);
    }

    _lastTime = currentTime;
}
