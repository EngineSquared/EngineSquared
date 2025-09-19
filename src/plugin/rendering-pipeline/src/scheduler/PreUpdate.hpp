#pragma once

#include "scheduler/Update.hpp"

namespace Plugin::RenderingPipeline {
/**
 * @brief Used for player's event handling like updating mouse position, buttons'state, etc.
 */
class PreUpdate : public Engine::Scheduler::Update {
  public:
    using Engine::Scheduler::Update::Update;
};
} // namespace Plugin::RenderingPipeline
