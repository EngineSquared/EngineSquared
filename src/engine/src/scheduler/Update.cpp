#include "Engine.pch.hpp"

#include "Update.hpp"
#include "Time.hpp"

void ES::Engine::Scheduler::Update::RunSystems()
{
    _elapsedTime = this->_core.GetResource<ES::Engine::Resource::Time>()._elapsedTime;

    for (auto const &system : this->GetSystems())
    {
        RunSystem(system.get(), _core);
    }
}
