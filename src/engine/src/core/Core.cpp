#include "Core.hpp"
#include "Entity.hpp"
#include "FixedTimeUpdate.hpp"
#include "Logger.hpp"
#include "RelativeTimeUpdate.hpp"
#include "Shutdown.hpp"
#include "Startup.hpp"

ES::Engine::Core::Core() : _registry(nullptr)
{
    ES::Utils::Log::Debug("Create Core");
    this->_registry = std::make_unique<entt::registry>();

    this->RegisterScheduler<ES::Engine::Scheduler::Startup>(
        [this]() { this->DeleteScheduler<ES::Engine::Scheduler::Startup>(); });
    this->RegisterScheduler<ES::Engine::Scheduler::Update>();
    this->RegisterScheduler<ES::Engine::Scheduler::FixedTimeUpdate>();
    this->RegisterScheduler<ES::Engine::Scheduler::RelativeTimeUpdate>();
    this->RegisterScheduler<ES::Engine::Scheduler::Shutdown>();
}

ES::Engine::Core::~Core() { ES::Utils::Log::Debug("Destroy Core"); }

ES::Engine::Entity ES::Engine::Core::CreateEntity()
{
    return static_cast<ES::Engine::Entity>(this->_registry->create());
}

void ES::Engine::Core::KillEntity(ES::Engine::Entity &entity)
{
    this->_registry->destroy(static_cast<entt::entity>(entity));
}

bool ES::Engine::Core::IsRunning() { return _running; }

void ES::Engine::Core::Stop()
{
    if (!_running)
    {
        ES::Utils::Log::Warn("The core is already shutted down");
        return;
    }
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
    this->_schedulers.RunSchedulers();

    for (const auto &scheduler : this->_schedulersToDelete)
    {
        this->_schedulers.DeleteScheduler(scheduler);
    }

    this->_schedulersToDelete.clear();
}

bool ES::Engine::Core::IsEntityValid(entt::entity entity) { return GetRegistry().valid(entity); }

void ES::Engine::Core::ClearEntities() { this->_registry->clear(); }

bool ES::Engine::Core::HasPlugin(std::type_index type) const { return this->_plugins.contains(type); }
