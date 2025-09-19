#pragma once

#include "scheduler/Update.hpp"

namespace Plugin::RenderingPipeline {
/**
 * @brief This class is used to swap the buffers and draw the data on the screen.
 */
class RenderSetup : public Engine::Scheduler::Update {
  public:
    using Engine::Scheduler::Update::Update;
};
} // namespace Plugin::RenderingPipeline
