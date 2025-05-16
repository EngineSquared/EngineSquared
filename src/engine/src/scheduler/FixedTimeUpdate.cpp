#include "Engine.pch.hpp"

#include "FixedTimeUpdate.hpp"

void ES::Engine::Scheduler::FixedTimeUpdate::RunSystems()
{
    auto currentTime = std::chrono::high_resolution_clock::now();
    _elapsedTime += std::chrono::duration<float>(currentTime - _lastTime).count();
    auto ticks = static_cast<unsigned int>(_elapsedTime / _tickRate);
    _elapsedTime -= ticks * _tickRate;

    for (unsigned int i = 0; i < ticks; i++)
    {
        for (auto const &system : this->GetSystems())
        {
            RunSystem(system.get(), _core);
        }
    }

    _lastTime = currentTime;
}
