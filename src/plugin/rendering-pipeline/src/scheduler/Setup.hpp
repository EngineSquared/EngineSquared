#pragma once

#include "scheduler/AScheduler.hpp"

namespace RenderingPipeline {
/**
 * @brief This class is used to setup libraries like GLFW, etc.
 */
class Setup : public Engine::Scheduler::AScheduler {
  public:
    using AScheduler::AScheduler;
    void RunSystems() override;
};
} // namespace RenderingPipeline
