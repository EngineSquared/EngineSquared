#pragma once

#include <entt/entt.hpp>

#include "AScheduler.hpp"

namespace ES::Plugin::RenderingPipeline {
// TODO: doc
class Setup : public ES::Engine::Scheduler::AScheduler {
  public:
    using AScheduler::AScheduler;
    void RunSystems() override;
};
} // namespace ES::Engine::Scheduler
