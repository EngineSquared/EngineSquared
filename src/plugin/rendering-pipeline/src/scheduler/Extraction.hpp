#pragma once

#include "scheduler/Update.hpp"

namespace ES::Plugin::RenderingPipeline {
/**
 * @brief This class is used to swap the buffers and draw the data on the screen.
 */
class Extraction : public ES::Engine::Scheduler::Update {
  public:
    using ES::Engine::Scheduler::Update::Update;
};
} // namespace ES::Plugin::RenderingPipeline
