#include "Registry.hpp"

entt::entity ES::Engine::Registry::CreateEntity() { return this->_registry->create(); }

void ES::Engine::Registry::RegisterSystem(USystem const &f, ES::Engine::ScheduleLabel label)
{
    this->_systems[label].push_back(f);
}

void ES::Engine::Registry::RunSystems()
{
    for (const auto &system : this->_systems[ScheduleLabel::NON_FIXED])
    {
        system(*this);
    }

    this->_fixedUpdateclock.Update();
    auto elapsedTicks = this->_fixedUpdateclock.GetElapsedTicks();

    for (unsigned int i = 0; i < elapsedTicks; i++)
    {
        for (const auto &system : this->_systems[ScheduleLabel::FIXED])
        {
            system(*this);
        }
    }
}

bool ES::Engine::Registry::IsEntityValid(entt::entity entity) { return GetRegistry().valid(entity); }

void ES::Engine::Registry::ClearEntities() { this->_registry->clear(); }
