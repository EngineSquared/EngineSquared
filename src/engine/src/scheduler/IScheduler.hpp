#pragma once

#include "System.hpp"
#include <entt/entt.hpp>

namespace ES::Engine {
class Core;
}

namespace ES::Engine::Scheduler {
using USystemList = std::vector<std::unique_ptr<SystemBase>>;
/**
 * @brief Interface to be implemented for every schedulers
 */
class IScheduler {
  public:
    virtual ~IScheduler() = default;
    /**
     * @brief Run the systems according to the scheduler policy
     *
     * @param systems The systems to run
     */
    virtual void RunSystems(void) = 0;
};
} // namespace ES::Engine::Scheduler
