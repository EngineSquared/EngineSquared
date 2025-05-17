#pragma once

#include "Update.hpp"

namespace ES::Plugin::RenderingPipeline {
/**
 * @brief Used to send the data to the GPU, like meshes, text, sprites, etc.
 */
class ToGPU : public ES::Engine::Scheduler::Update {
  public:
    using ES::Engine::Scheduler::Update::Update;
};
} // namespace ES::Plugin::RenderingPipeline
