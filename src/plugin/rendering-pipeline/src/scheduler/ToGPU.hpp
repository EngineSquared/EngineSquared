#pragma once

#include "scheduler/Update.hpp"

namespace Plugin::RenderingPipeline {
/**
 * @brief Used to send the data to the GPU, like meshes, text, sprites, etc.
 */
class ToGPU : public Engine::Scheduler::Update {
  public:
    using Engine::Scheduler::Update::Update;
};
} // namespace Plugin::RenderingPipeline
