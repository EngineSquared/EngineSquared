#include "Registry.hpp"

ES::Engine::Registry::Registry() : _registry(nullptr)
{
    this->_registry = std::make_unique<entt::registry>();
    this->_systems = std::unordered_map<ScheduleLabel, std::vector<USystem>>();
    this->_systems[ScheduleLabel::NON_FIXED] = std::vector<USystem>();
    this->_systems[ScheduleLabel::FIXED] = std::vector<USystem>();
    this->_fixedUpdateclock = ES::Utils::Clock();
}

entt::entity ES::Engine::Registry::CreateEntity() { return this->_registry->create(); }

void ES::Engine::Registry::RegisterSystem(USystem const &f, ES::Engine::ScheduleLabel label)
{
    this->_systems[label].push_back(f);
}

void ES::Engine::Registry::RunSystems()
{
    for (auto &system : this->_systems[ScheduleLabel::NON_FIXED])
    {
        system(*this);
    }

    this->_fixedUpdateclock.Update();
    auto elapsedTicks = this->_fixedUpdateclock.GetElapsedTicks();

    for (unsigned int i = 0; i < elapsedTicks; i++)
    {
        for (auto &system : this->_systems[ScheduleLabel::FIXED])
        {
            system(*this);
        }
    }
}

bool ES::Engine::Registry::IsEntityValid(entt::entity entity) { return GetRegistry().valid(entity); }

void ES::Engine::Registry::ClearEntities() { this->_registry->clear(); }
