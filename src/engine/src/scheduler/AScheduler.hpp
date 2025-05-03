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

  protected:
    Core &_core;
    SystemContainer _systemsList;
};
} // namespace ES::Engine::Scheduler
