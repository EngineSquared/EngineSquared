#include "Update.hpp"

void ES::Engine::Scheduler::Update::RunSystems()
{
    auto currentTime = std::chrono::high_resolution_clock::now();
    _elapsedTime = std::chrono::duration<float>(currentTime - _lastTime).count();
    _lastTime = currentTime;

    for (auto const &system : this->GetSystems())
    {
        RunSystem(system, _core);
    }
}
