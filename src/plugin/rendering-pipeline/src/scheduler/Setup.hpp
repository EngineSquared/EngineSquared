#pragma once

#include "scheduler/AScheduler.hpp"

namespace ES::Plugin::RenderingPipeline {
/**
 * @brief This class is used to setup libraries like GLFW, GLEW, etc.
 */
class Setup : public ES::Engine::Scheduler::AScheduler {
  public:
    Setup(ES::Engine::Core &core) : AScheduler(core){};

    void RunSystems() override;
};
} // namespace ES::Plugin::RenderingPipeline
