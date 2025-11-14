#pragma once

#include "resource/ARenderPass.hpp"

namespace Graphic::Resource {

class SingleExecutionRenderPass : public ARenderPass<SingleExecutionRenderPass> {
  public:
    SingleExecutionRenderPass(std::string_view name) : ARenderPass(name) {}

    void Execute(Engine::Core &core) override {}

  private:
    // wgpu::RenderPassEncoder _CreateRenderPass(Engine::Core &core)
    // {

    // }

    std::optional<std::function<void(wgpu::RenderPassEncoder &renderPass, Engine::Core &core)>> uniqueRenderCallback =
        std::nullopt;
};
} // namespace Graphic::Resource
