#pragma once

#include <entt/entt.hpp>

#include "Update.hpp"

namespace ES::Plugin::RenderingPipeline {
  /**
   * @brief Setup the rendering pipeline, like clearing the screen, setting up the camera, etc.
   */
class RenderSetup : public ES::Engine::Scheduler::Update {
  public:
    using ES::Engine::Scheduler::Update::Update;
};
} // namespace ES::Plugin::RenderingPipeline
