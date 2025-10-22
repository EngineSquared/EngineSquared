#pragma once

#include "scheduler/Update.hpp"

namespace RenderingPipeline {
/**
 * @brief This class is used to swap the buffers and draw the data on the screen.
 */
class PipelineExecution : public Engine::Scheduler::Update {
  public:
    using Engine::Scheduler::Update::Update;
};
} // namespace RenderingPipeline
