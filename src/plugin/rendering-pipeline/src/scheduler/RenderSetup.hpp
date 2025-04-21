#pragma once

#include <entt/entt.hpp>

#include "AScheduler.hpp"

namespace ES::Plugin::RenderingPipeline {
class RenderSetup : public ES::Engine::Scheduler::Update {
  public:
    using ES::Engine::Scheduler::Update::Update;
};
} // namespace ES::Engine::Scheduler
