#pragma once

#include "scheduler/AScheduler.hpp"

namespace RenderingPipeline {
/**
 * @brief This class is used to initialize libraries like GLFW, etc.
 */
class Init : public Engine::Scheduler::AScheduler {
  public:
    using AScheduler::AScheduler;
    void RunSystems() override;
};
} // namespace RenderingPipeline
