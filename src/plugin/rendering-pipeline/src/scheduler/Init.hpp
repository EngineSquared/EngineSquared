#pragma once

#include "scheduler/AScheduler.hpp"

namespace ES::Plugin::RenderingPipeline {
/**
 * @brief This class is used to initialize libraries like GLFW, GLEW, etc.
 */
class Init : public ES::Engine::Scheduler::AScheduler {
  public:
    Init(ES::Engine::Core &core) : AScheduler(core) {};

    void RunSystems() override;
};
} // namespace ES::Plugin::RenderingPipeline
