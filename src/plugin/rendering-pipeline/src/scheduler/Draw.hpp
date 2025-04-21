#pragma once

#include <entt/entt.hpp>

#include "Update.hpp"

namespace ES::Plugin::RenderingPipeline {
/**
 * @brief Update scheduler that runs systems every time it is called
 */
class Draw : public ES::Engine::Scheduler::Update {
  public:
    using ES::Engine::Scheduler::Update::Update;
};
} // namespace ES::Plugin::RenderingPipeline
