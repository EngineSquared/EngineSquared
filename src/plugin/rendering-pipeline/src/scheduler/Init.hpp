#pragma once

#include "AScheduler.hpp"

namespace ES::Plugin::RenderingPipeline {
/**
 * @brief This class is used to initialize libraries like GLFW, GLEW, etc.
 */
class Init : public ES::Engine::Scheduler::AScheduler {
  public:
    using AScheduler::AScheduler;
    void RunSystems() override;
};
} // namespace ES::Plugin::RenderingPipeline
