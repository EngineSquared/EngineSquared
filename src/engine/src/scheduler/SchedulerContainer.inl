#include "SchedulerContainer.hpp"

template <typename TScheduler, typename... Args>
void ES::Engine::SchedulerContainer::AddScheduler(Core &core, Args &&...args)
{
    if (this->_schedulers.contains(std::type_index(typeid(TScheduler))))
    {
        ES::Utils::Log::Warn(fmt::format("Scheduler already exists: {}", typeid(TScheduler).name()));
        return;
    }
#ifdef ES_DEBUG
    ES::Utils::Log::Info(fmt::format("Adding scheduler: {}", typeid(TScheduler).name()));
#endif
    std::shared_ptr<TScheduler> scheduler = std::make_shared<TScheduler>(core, std::forward<Args>(args)...);
    this->_schedulers[std::type_index(typeid(TScheduler))] = scheduler;
    this->_orderedSchedulers.push_back(scheduler);
}

template <typename TScheduler> inline TScheduler &ES::Engine::SchedulerContainer::GetScheduler()
{
    auto it = this->_schedulers.find(std::type_index(typeid(TScheduler)));
    if (it == this->_schedulers.end())
    {
        throw SchedulerError(fmt::format("Scheduler not found: {}", typeid(TScheduler).name()));
    }
    return *static_cast<TScheduler *>(it->second.get());
}
