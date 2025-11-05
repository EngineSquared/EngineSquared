#pragma once

#include "scheduler/Update.hpp"

namespace RenderingPipeline {
/**
 * @brief Used for player's event handling like updating mouse position, buttons'state, etc.
 */
class Batching : public Engine::Scheduler::Update {
  public:
    using Engine::Scheduler::Update::Update;
};
} // namespace RenderingPipeline
