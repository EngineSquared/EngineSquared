#include "Core.hpp"
#include "Entity.hpp"
#include "Logger.hpp"

ES::Engine::Core::Core() : _registry(nullptr), _running(false)
{
    this->_registry = std::make_unique<entt::registry>();

    this->RegisterScheduler<ES::Engine::Scheduler::Startup>(
        [this]() { this->DeleteScheduler<ES::Engine::Scheduler::Startup>(); });
    this->RegisterScheduler<ES::Engine::Scheduler::Update>();
    this->RegisterScheduler<ES::Engine::Scheduler::FixedTimeUpdate>();
    this->RegisterScheduler<ES::Engine::Scheduler::RelativeTimeUpdate>();
}

ES::Engine::Entity ES::Engine::Core::CreateEntity()
{
    return static_cast<ES::Engine::Entity>(this->_registry->create());
}

bool ES::Engine::Core::IsRunning() { return _running; }

void ES::Engine::Core::Stop()
{
    if (!_running)
        ES::Utils::Log::Warn("The core is already shutted down");
    _running = false;
}

void ES::Engine::Core::RunCore()
{
    _running = true;
    while (_running)
    {
        RunSystems();
    }
}

void ES::Engine::Core::RunSystems()
{

    for (auto &[schedulerIndex, scheduler] : this->_schedulers)
    {
        scheduler->RunSystems(this->_systems[schedulerIndex]);
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
