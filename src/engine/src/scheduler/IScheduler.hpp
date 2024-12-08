#pragma once

#include <entt/entt.hpp>

namespace ES::Engine {
class Registry;
}

namespace ES::Engine::Scheduler {
using USystem = std::function<void(Registry &)>;

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
    virtual void RunSystems(std::vector<USystem> systems) = 0;
};
} // namespace ES::Engine::Scheduler
