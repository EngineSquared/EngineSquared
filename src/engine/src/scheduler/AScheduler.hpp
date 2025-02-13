#pragma once

#include "IScheduler.hpp"

namespace ES::Engine::Scheduler {
/**
 * @brief Interface to be implemented for every schedulers
 */
class AScheduler : public IScheduler {
  public:
    explicit AScheduler(Core &registry) : _registry(registry) {}

  protected:
    Core &_registry;
};
} // namespace ES::Engine::Scheduler
