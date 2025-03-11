#pragma once

#include <entt/entt.hpp>

#include "AScheduler.hpp"

namespace ES::Engine::Scheduler {
/**
 * @brief Update scheduler that runs systems every time it is called
 */
class Update : public AScheduler {
  public:
    explicit Update(Core &registry) : AScheduler(registry) {}
    void RunSystems(std::vector<std::unique_ptr<SystemBase>> &systems) override;
};
} // namespace ES::Engine::Scheduler
