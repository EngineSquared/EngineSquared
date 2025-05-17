#include "Engine.pch.hpp"

#include "RelativeTimeUpdate.hpp"
#include "Time.hpp"

void ES::Engine::Scheduler::RelativeTimeUpdate::RunSystems()
{
    _bufferedTime += this->_core.GetResource<ES::Engine::Resource::Time>()._elapsedTime;
    auto ticks = static_cast<unsigned int>(_bufferedTime / _tickRate);
    float remainder = _bufferedTime - ticks * _tickRate;
    _bufferedTime -= ticks * _tickRate;

    for (unsigned int i = 0; i < ticks; i++)
    {
        _deltaTime = _tickRate;
        for (auto const &system : this->GetSystems())
        {
            RunSystem(system.get(), _core);
        }
    }

    if (remainder > REMAINDER_THRESHOLD)
    {
        _deltaTime = remainder;
        _bufferedTime = 0.0f;
        for (auto const &system : this->GetSystems())
        {
            RunSystem(system.get(), _core);
        }
    }
}
