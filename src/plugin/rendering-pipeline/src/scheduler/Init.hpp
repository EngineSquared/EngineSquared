#pragma once

#include <entt/entt.hpp>

#include "AScheduler.hpp"

namespace ES::Plugin::RenderingPipeline {
/**
 * @brief Update scheduler that runs systems every time it is called
 */
class Init : public ES::Engine::Scheduler::AScheduler {
  public:
    using AScheduler::AScheduler;
    void RunSystems() override;
};
} // namespace ES::Plugin::RenderingPipeline
