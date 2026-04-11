#include "Engine.pch.hpp"

#include "Update.hpp"
#include "resource/Time.hpp"

float Engine::Scheduler::Update::GetDeltaTime() const { return _elapsedTime; }

void Engine::Scheduler::Update::RunSystems()
{
    _elapsedTime = this->_core.GetResource<Engine::Resource::Time>()._elapsedTime;

    for (auto const &system : this->GetSystems())
    {
        RunSystem(system.get(), _core);
    }
}
