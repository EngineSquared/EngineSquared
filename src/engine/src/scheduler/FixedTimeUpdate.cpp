#include "Engine.pch.hpp"

#include "FixedTimeUpdate.hpp"
#include "Time.hpp"

void ES::Engine::Scheduler::FixedTimeUpdate::RunSystems()
{
    _bufferedTime += this->_core.GetResource<ES::Engine::Resource::Time>()._elapsedTime;
    auto ticks = static_cast<unsigned int>(_bufferedTime / _tickRate);
    _bufferedTime -= ticks * _tickRate;

    for (unsigned int i = 0; i < ticks; i++)
    {
        for (auto const &system : this->GetSystems())
        {
            RunSystem(system.get(), _core);
        }
    }
}
