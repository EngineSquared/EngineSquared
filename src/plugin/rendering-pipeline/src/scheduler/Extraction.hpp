#pragma once

#include "scheduler/Update.hpp"

namespace RenderingPipeline {
/**
 * @brief Used for player's event handling like updating mouse position, buttons'state, etc.
 */
class Extraction : public Engine::Scheduler::Update {
  public:
    inline static const Engine::SchedulerCategory Category = Engine::SchedulerCategory::Runtime;
    using Engine::Scheduler::Update::Update;
};
} // namespace RenderingPipeline
