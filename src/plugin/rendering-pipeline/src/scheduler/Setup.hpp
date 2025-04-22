#pragma once

#include <entt/entt.hpp>

#include "AScheduler.hpp"

namespace ES::Plugin::RenderingPipeline {
/**
 * @brief This class is used to setup libraries like GLFW, GLEW, etc.
 */
class Setup : public ES::Engine::Scheduler::AScheduler {
  public:
    using AScheduler::AScheduler;
    void RunSystems() override;
};
} // namespace ES::Plugin::RenderingPipeline
