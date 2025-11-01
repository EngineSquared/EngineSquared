#pragma once

#include "resource/ARenderPass.hpp"

namespace Graphic::Resource {

class SingleExecutionRenderPass : public ARenderPass<SingleExecutionRenderPass> {
  public:
    SingleExecutionRenderPass(std::string_view name) : ARenderPass(name) {}

    SingleExecutionRenderPass &Execute(Engine::Core &core) override
    {
        // Implementation of the single execution render pass logic goes here.
        // This is a placeholder for demonstration purposes.
        return *this;
    }
};
} // namespace Graphic::Resource
