#pragma once

#include "IScheduler.hpp"

namespace ES::Engine::Scheduler {
/**
 * @brief Interface to be implemented for every schedulers
 */
class AScheduler : public IScheduler {
  public:
    explicit AScheduler(Registry &registry) : _registry(registry) {}
    /**
     * @brief Run the systems according to the scheduler policy
     *
     * @param systems The systems to run
     * @see USystem
     */
    virtual void RunSystems(std::vector<USystem> systems) = 0;

  protected:
    Registry &_registry;
};
} // namespace ES::Engine::Scheduler
