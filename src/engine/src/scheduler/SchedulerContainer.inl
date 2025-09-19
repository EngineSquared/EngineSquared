#include "SchedulerContainer.hpp"

template <typename TScheduler, typename... Args>
void Engine::SchedulerContainer::AddScheduler(Core &core, Args &&...args)
{
    if (this->_schedulers.contains(std::type_index(typeid(TScheduler))))
    {
        Log::Warn(fmt::format("Scheduler already exists: {}", typeid(TScheduler).name()));
        return;
    }
    Log::Debug(fmt::format("Adding scheduler: {}", typeid(TScheduler).name()));
    std::shared_ptr<TScheduler> scheduler = std::make_shared<TScheduler>(core, std::forward<Args>(args)...);
    this->_schedulers[std::type_index(typeid(TScheduler))] = scheduler;
    this->_orderedSchedulers.push_back(scheduler);
}

template <typename TScheduler> inline TScheduler &Engine::SchedulerContainer::GetScheduler()
{
    auto it = this->_schedulers.find(std::type_index(typeid(TScheduler)));
    if (it == this->_schedulers.end())
    {
        throw SchedulerError(fmt::format("Scheduler not found: {}", typeid(TScheduler).name()));
    }
    return *static_cast<TScheduler *>(it->second.get());
}

template <typename TBefore, typename TAfter> inline void Engine::SchedulerContainer::Before()
{
    _dirty = true;
    _dependencies[std::type_index(typeid(TAfter))].insert(std::type_index(typeid(TBefore)));
}

template <typename TAfter, typename TBefore> inline void Engine::SchedulerContainer::After()
{
    Before<TBefore, TAfter>();
}

template <typename TBefore, typename TAfter> void Engine::SchedulerContainer::RemoveDependencyBefore()
{
    _dirty = true;
    auto it = _dependencies.find(std::type_index(typeid(TAfter)));
    if (it != _dependencies.end())
    {
        it->second.erase(std::type_index(typeid(TBefore)));
        if (it->second.empty())
            _dependencies.erase(it);
    }
    else
    {
        Log::Warn(
            fmt::format("Dependency not found: {} -> {}", typeid(TBefore).name(), typeid(TAfter).name()));
    }
}

template <typename TAfter, typename TBefore> void Engine::SchedulerContainer::RemoveDependencyAfter()
{
    RemoveDependencyBefore<TBefore, TAfter>();
}

inline void Engine::SchedulerContainer::RunSchedulers()
{
    Update();
    for (const auto &scheduler : _orderedSchedulers)
    {
        scheduler->RunSystems();

        if (!scheduler->ShouldRunNextScheduler())
        {
            break;
        }
    }
}

inline bool Engine::SchedulerContainer::Contains(std::type_index id) const
{
    return this->_schedulers.contains(id);
}

template <typename TScheduler> inline bool Engine::SchedulerContainer::Contains() const
{
    return Contains(std::type_index(typeid(TScheduler)));
}

inline std::shared_ptr<Engine::Scheduler::AScheduler>
Engine::SchedulerContainer::GetScheduler(std::type_index id)
{
    auto it = this->_schedulers.find(id);
    if (it == this->_schedulers.end())
        throw SchedulerError(fmt::format("Scheduler not found: {}", id.name()));
    return it->second;
}
