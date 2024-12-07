#pragma once

#include "IScheduler.hpp"

namespace ES::Engine::Scheduler {
/**
 * @brief Interface to be implemented for every schedulers
 */
class AScheduler : public IScheduler {
  public:
    explicit AScheduler(Registry &registry) : _registry(registry) {}

  protected:
    Registry &_registry;
};
} // namespace ES::Engine::Scheduler
