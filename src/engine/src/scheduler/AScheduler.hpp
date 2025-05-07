#pragma once

#include "IScheduler.hpp"
#include <set>
#include <typeindex>

namespace ES::Engine::Scheduler {
/**
 * @brief Interface to be implemented for every schedulers
 */
class AScheduler : public IScheduler {
  public:
    explicit AScheduler(Core &core) : _core(core) {}

    template <typename... Systems> decltype(auto) AddSystems(Systems... systems)
    {
        return _systemsList.AddSystems(systems...);
    }

    inline void Disable(const ES::Utils::FunctionContainer::FunctionID &id) { _systemsList.Disable(id); }

    inline void Enable(const ES::Utils::FunctionContainer::FunctionID &id) { _systemsList.Enable(id); }

  protected:
    Core &_core;
    SystemContainer _systemsList;
};
} // namespace ES::Engine::Scheduler
