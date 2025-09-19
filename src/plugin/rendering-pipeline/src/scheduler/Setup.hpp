#pragma once

#include "scheduler/AScheduler.hpp"

namespace Plugin::RenderingPipeline {
/**
 * @brief This class is used to setup libraries like GLFW, GLEW, etc.
 */
class Setup : public Engine::Scheduler::AScheduler {
  public:
    using AScheduler::AScheduler;
    void RunSystems() override;
};
} // namespace Plugin::RenderingPipeline
