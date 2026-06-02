#pragma once

#include "scheduler/SchedulerCategory.hpp"
#include "scheduler/SchedulerContainer.hpp"
#include <unordered_map>

template <class TScheduler>
concept CHasCategory = requires(TScheduler) { TScheduler::Category; };

namespace Engine {
class SchedulingContext {
  public:
    template <typename TScheduler, typename... Args> void AddScheduler(Core &core, Args &&...args)
    {
        _schedulerContainers[GetCategory<TScheduler>()].template AddScheduler<TScheduler>(core,
                                                                                          std::forward<Args>(args)...);
    }

    template <typename TScheduler> TScheduler &GetScheduler(void)
    {
        return _schedulerContainers.at(GetCategory<TScheduler>()).template GetScheduler<TScheduler>();
    }

    std::shared_ptr<Scheduler::AScheduler> GetScheduler(std::type_index id)
    {
        for (auto &[_, schedulerContainer] : _schedulerContainers)
        {
            if (schedulerContainer.Contains(id))
                return schedulerContainer.GetScheduler(id);
        }
        return nullptr;
    }

    template <typename TScheduler> bool Contains() const
    {
        _schedulerContainers.at(GetCategory<TScheduler>()).template Contains<TScheduler>();
    }

    bool Contains(std::type_index id) const
    {
        for (auto &[_, schedulerContainer] : _schedulerContainers)
        {
            if (schedulerContainer.Contains(id))
                return true;
        }
        return false;
    }

    template <typename TBefore, typename TAfter> void Before()
    {
        const auto beforeSchedulerCategory = GetCategory<TBefore>();
        _schedulerContainers.at(beforeSchedulerCategory).template Before<TBefore, TAfter>();
    }

    template <typename TAfter, typename TBefore> void After()
    {
        const auto beforeSchedulerCategory = GetCategory<TAfter>();
        _schedulerContainers.at(beforeSchedulerCategory).template After<TAfter, TBefore>();
    }

    template <typename TBefore, typename TAfter> void RemoveDependencyBefore(void)
    {
        const auto beforeSchedulerCategory = GetCategory<TBefore>();
        _schedulerContainers.at(beforeSchedulerCategory).template RemoveDependencyBefore<TBefore, TAfter>();
    }

    template <typename TAfter, typename TBefore> void RemoveDependencyAfter(void)
    {
        const auto beforeSchedulerCategory = GetCategory<TAfter>();
        _schedulerContainers.at(beforeSchedulerCategory).template RemoveDependencyAfter<TAfter, TBefore>();
    }

    bool RunSchedulers(SchedulerCategory category)
    {
        if (_schedulerContainers.contains(category))
            return _schedulerContainers.at(category).RunSchedulers();
        return true;
    }

    void SetErrorPolicyForAllSchedulers(Scheduler::SchedulerErrorPolicy policy)
    {
        for (auto &[_, schedulerContainer] : _schedulerContainers)
        {
            schedulerContainer.SetErrorPolicyForAllSchedulers(policy);
        }
    }

    template <typename TScheduler> void DeleteScheduler()
    {
        _schedulerContainers.at(GetCategory<TScheduler>()).template DeleteScheduler<TScheduler>();
    }

    void DeleteScheduler(std::type_index id)
    {
        for (auto &[_, schedulerContainer] : _schedulerContainers)
        {
            if (schedulerContainer.Contains(id))
            {
                schedulerContainer.DeleteScheduler(id);
                return;
            }
        }
    }

  private:
    template <CHasCategory TScheduler> static SchedulerCategory GetCategory(void) { return TScheduler::Category; }

    std::unordered_map<SchedulerCategory, SchedulerContainer> _schedulerContainers;
};
} // namespace Engine
