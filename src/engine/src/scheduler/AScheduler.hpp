#pragma once

#include "IScheduler.hpp"

namespace ES::Engine::Scheduler {
/**
 * @brief Interface to be implemented for every schedulers
 */
class AScheduler : public IScheduler {
  public:
    explicit AScheduler(Core &core) : _core(core) {}

    template <typename... Systems> void AddSystems(Systems... systems) { _systemsList.AddSystems(systems...); }

  protected:
    Core &_core;
    SystemContainer _systemsList;
};
} // namespace ES::Engine::Scheduler
