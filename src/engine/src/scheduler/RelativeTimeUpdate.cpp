#include "RelativeTimeUpdate.hpp"

void ES::Engine::Scheduler::RelativeTimeUpdate::operator()(std::vector<USystem> systems)
{
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto diff = std::chrono::duration<float>(currentTime - _lastTime).count();
    auto ticks = static_cast<unsigned int>(diff / _tickRate);
    float remainder = diff - ticks * _tickRate;

    for (unsigned int i = 0; i < ticks; i++)
    {
        _deltaTime = _tickRate;
        for (auto &system : systems)
        {
            system(_registry);
        }
    }

    if (remainder > REMAINDER_THRESHOLD)
    {
        _deltaTime = remainder;
        for (auto &system : systems)
        {
            system(_registry);
        }
    }

    _lastTime = currentTime;
}