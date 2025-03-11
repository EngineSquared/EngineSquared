#pragma once

#include <entt/entt.hpp>
#include "System.hpp"

namespace ES::Engine {
class Core;
}

namespace ES::Engine::Scheduler {
using USystem = std::function<void(Core &)>;

/**
 * @brief Interface to be implemented for every schedulers
 */
class IScheduler {
  public:
    /**
     * @brief Run the systems according to the scheduler policy
     *
     * @param systems The systems to run
     */
    virtual void RunSystems(std::vector<std::unique_ptr<SystemBase>> /* TODO: Use "using" defined type*/ &systems) = 0;
};
} // namespace ES::Engine::Scheduler
