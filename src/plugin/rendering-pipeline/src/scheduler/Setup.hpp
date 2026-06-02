#pragma once

#include "scheduler/AScheduler.hpp"
#include "scheduler/SchedulerCategory.hpp"

namespace RenderingPipeline {
/**
 * @brief This class is used to setup libraries like GLFW, etc.
 */
class Setup : public Engine::Scheduler::AScheduler {
  public:
    inline static const Engine::SchedulerCategory Category = Engine::SchedulerCategory::Startup;
    using AScheduler::AScheduler;
    void RunSystems() override;
};
} // namespace RenderingPipeline
