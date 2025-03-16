#pragma once

#include "IScheduler.hpp"

namespace ES::Engine::Scheduler {
/**
 * @brief Interface to be implemented for every schedulers
 */
class AScheduler : public IScheduler {
  public:
    explicit AScheduler(Core &registry) : _registry(registry) {}

    template <typename... Systems> void AddSystems(Systems... systems) { _systemsList.AddSystems(systems...); }

  protected:
    Core &_registry;
    SystemContainer _systemsList;
};
} // namespace ES::Engine::Scheduler
