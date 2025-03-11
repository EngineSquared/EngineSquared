#include "Core.hpp"

ES::Engine::Core::Core() : _registry(nullptr)
{
    this->_registry = std::make_unique<entt::registry>();

    this->RegisterScheduler<ES::Engine::Scheduler::Startup>(
        [this]() { this->DeleteScheduler<ES::Engine::Scheduler::Startup>(); });
    this->RegisterScheduler<ES::Engine::Scheduler::Update>();
    this->RegisterScheduler<ES::Engine::Scheduler::FixedTimeUpdate>();
    this->RegisterScheduler<ES::Engine::Scheduler::RelativeTimeUpdate>();
}

entt::entity ES::Engine::Core::CreateEntity() { return this->_registry->create(); }

void ES::Engine::Core::RunSystems()
{

    for (auto &[schedulerIndex, scheduler] : this->_schedulers)
    {
        scheduler->RunSystems(this->_systems[schedulerIndex].GetSystems());
    }

    for (auto &scheduler : this->_schedulersToDelete)
    {
        this->_schedulers.erase(scheduler);
        this->_systems.erase(scheduler);
    }

    this->_schedulersToDelete.clear();
}

bool ES::Engine::Core::IsEntityValid(entt::entity entity) { return GetRegistry().valid(entity); }

void ES::Engine::Core::ClearEntities() { this->_registry->clear(); }
