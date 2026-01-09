#include "Engine.pch.hpp"

#include "core/Core.hpp"
#include "entity/Entity.hpp"
#include "resource/Time.hpp"
#include "scheduler/FixedTimeUpdate.hpp"
#include "scheduler/RelativeTimeUpdate.hpp"
#include "scheduler/Shutdown.hpp"
#include "scheduler/Startup.hpp"

Engine::Core::Core() : _registry(nullptr)
{
    Log::Debug("Create Core");
    this->_registry = std::make_unique<entt::registry>();

    this->RegisterResource<Resource::Time>(Resource::Time());

this->RegisterScheduler<Scheduler::Startup>([this]() { this->DeleteScheduler<Scheduler::Startup>(); });

    this->RegisterScheduler<Scheduler::Update>();
    this->RegisterScheduler<Scheduler::FixedTimeUpdate>();
    this->RegisterScheduler<Scheduler::RelativeTimeUpdate>();
    this->RegisterScheduler<Scheduler::Shutdown>();

    // Calling Update at startup ensure that the time is not 0 when calling the update scheduler
    this->RegisterSystem<Scheduler::Startup>(Resource::Time::Update);
    this->RegisterSystem<Scheduler::Update>(Resource::Time::Update);

    this->SetSchedulerBefore<Scheduler::Startup, Scheduler::Update>();
    this->SetSchedulerBefore<Scheduler::Startup, Scheduler::FixedTimeUpdate>();
    this->SetSchedulerBefore<Scheduler::Startup, Scheduler::RelativeTimeUpdate>();

    // As update scheduler update time, it should be before the fixed time update and relative time update
    this->SetSchedulerBefore<Scheduler::Update, Scheduler::FixedTimeUpdate>();
    this->SetSchedulerBefore<Scheduler::Update, Scheduler::RelativeTimeUpdate>();

    this->SetSchedulerAfter<Scheduler::Shutdown, Scheduler::Update>();
    this->SetSchedulerAfter<Scheduler::Shutdown, Scheduler::FixedTimeUpdate>();
    this->SetSchedulerAfter<Scheduler::Shutdown, Scheduler::RelativeTimeUpdate>();
}

Engine::Core::~Core() { Log::Debug("Destroy Core"); }

Engine::Entity Engine::Core::CreateEntity() { return static_cast<Engine::Entity>(this->_registry->create()); }

void Engine::Core::KillEntity(Engine::Entity &entity) { this->_registry->destroy(static_cast<entt::entity>(entity)); }

bool Engine::Core::IsRunning() { return _running; }

void Engine::Core::Stop()
{
    if (!_running)
    {
        Log::Warn("The core is already shutted down");
        return;
    }
    _running = false;
}

void Engine::Core::RunCore()
{
    _running = true;
    while (_running)
    {
        RunSystems();
    }
}

void Engine::Core::RunSystems()
{
    this->_schedulers.RunSchedulers();

    for (const auto &scheduler : this->_schedulersToDelete)
    {
        this->_schedulers.DeleteScheduler(scheduler);
    }

    this->_schedulersToDelete.clear();
}

bool Engine::Core::IsEntityValid(entt::entity entity) { return GetRegistry().valid(entity); }

void Engine::Core::ClearEntities() { this->_registry->clear(); }

bool Engine::Core::HasPlugin(std::type_index type) const { return this->_plugins.contains(type); }

void Engine::Core::SetErrorPolicyForAllSchedulers(Scheduler::SchedulerErrorPolicy policy)
{
    _schedulers.SetErrorPolicyForAllSchedulers(policy);
}
