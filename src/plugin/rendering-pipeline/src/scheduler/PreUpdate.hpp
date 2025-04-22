#pragma once

#include <entt/entt.hpp>

#include "Update.hpp"

namespace ES::Plugin::RenderingPipeline {
/**
 * @brief Used for player's event handling like updating mouse position, buttons'state, etc.
 */
class PreUpdate : public ES::Engine::Scheduler::Update {
  public:
    using ES::Engine::Scheduler::Update::Update;
};
} // namespace ES::Plugin::RenderingPipeline
