#pragma once

#include <entt/entt.hpp>

#include "AScheduler.hpp"

namespace ES::Engine::Scheduler {
/**
 * @brief Update scheduler that runs systems every time it is called
 */
class Update : public AScheduler {
  public:
    using AScheduler::AScheduler;
    virtual ~Update() = default;
    void RunSystems(std::vector<std::unique_ptr<SystemBase>> &systems) override;
};
} // namespace ES::Engine::Scheduler
