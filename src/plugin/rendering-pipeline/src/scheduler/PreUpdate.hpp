#pragma once

#include <entt/entt.hpp>

#include "AScheduler.hpp"

namespace ES::Plugin::RenderingPipeline {
// TODO: doc
class PreUpdate : public ES::Engine::Scheduler::Update {
  public:
  using ES::Engine::Scheduler::Update::Update;
};
}
