#include "Registry.hpp"

ES::Engine::Registry::Registry() : _registry(nullptr)
{
    this->_registry = std::make_unique<entt::registry>();

    this->RegisterScheduler<ES::Engine::Scheduler::Startup>();
    this->RegisterScheduler<ES::Engine::Scheduler::Update>();
    this->RegisterScheduler<ES::Engine::Scheduler::FixedTimeUpdate>();
    this->RegisterScheduler<ES::Engine::Scheduler::RelativeTimeUpdate>();
}

entt::entity ES::Engine::Registry::CreateEntity() { return this->_registry->create(); }

void ES::Engine::Registry::RunSystems()
{

    for (auto &scheduler : this->_schedulers)
    {
        scheduler.second->operator()(this->_systems[scheduler.first]);
    }
}

bool ES::Engine::Registry::IsEntityValid(entt::entity entity) { return GetRegistry().valid(entity); }

void ES::Engine::Registry::ClearEntities() { this->_registry->clear(); }
