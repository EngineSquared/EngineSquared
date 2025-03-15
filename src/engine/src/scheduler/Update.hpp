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
    void RunSystems() override;
};
} // namespace ES::Engine::Scheduler
