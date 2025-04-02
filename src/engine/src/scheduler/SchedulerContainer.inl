#include "SchedulerContainer.hpp"

template <typename TScheduler, typename... Args>
void ES::Engine::SchedulerContainer::AddScheduler(Core &core, Args &&...args)
{
    if (_idToIndex.contains(std::type_index(typeid(TScheduler))))
    {
        ES::Utils::Log::Warn(fmt::format("Scheduler already exists: {}", typeid(TScheduler).name()));
        return;
    }
#ifdef ES_DEBUG
    ES::Utils::Log::Info(fmt::format("Adding scheduler: {}", typeid(TScheduler).name()));
#endif
    std::size_t index = _orderedSchedulers.size();
    auto schedulerPtr = std::make_unique<TScheduler>(core, std::forward<Args>(args)...);
    _orderedSchedulers.push_back(std::move(schedulerPtr));
    _idToIndex[std::type_index(typeid(TScheduler))] = index;
}

template <typename TScheduler> inline TScheduler &ES::Engine::SchedulerContainer::GetScheduler()
{
    auto it = _idToIndex.find(std::type_index(typeid(TScheduler)));
    if (it == _idToIndex.end())
    {
        throw SchedulerError(fmt::format("Scheduler not found: {}", typeid(TScheduler).name()));
    }
    return *static_cast<TScheduler *>(_orderedSchedulers[it->second].get());
}
