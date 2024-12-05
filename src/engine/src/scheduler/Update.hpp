#pragma once

#include <entt/entt.hpp>

#include "IScheduler.hpp"

namespace ES::Engine {
class Registry;
}

namespace ES::Engine::Scheduler {
using USystem = std::function<void(Registry &)>;

/**
 * @brief Update scheduler that runs systems every time it is called
 */
class Update : public IScheduler {
  public:
    explicit Update(Registry &registry) : IScheduler(registry) {}
    void operator()(std::vector<USystem> systems) override
    {
        for (auto &system : systems)
        {
            system(_registry);
        }
    }
};
} // namespace ES::Engine::Scheduler
