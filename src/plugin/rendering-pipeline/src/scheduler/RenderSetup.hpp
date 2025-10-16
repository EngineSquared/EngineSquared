#pragma once

#include "scheduler/Update.hpp"

namespace RenderingPipeline {
/**
 * @brief Setup the rendering pipeline, like clearing the screen, setting up the camera, etc.
 */
class RenderSetup : public Engine::Scheduler::Update {
  public:
    using Engine::Scheduler::Update::Update;
};
} // namespace RenderingPipeline
