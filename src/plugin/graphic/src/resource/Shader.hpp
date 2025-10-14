#pragma once

#include "utils/BindGroupLayout.hpp"
#include "utils/ColorTargetState.hpp"
#include "utils/DepthStencilState.hpp"
#include "utils/VertexBufferLayout.hpp"
#include "utils/webgpu.hpp"
#include <vector>

namespace Plugin::Graphic::Resource {

class Shader {
  public:
    Shader() = default;
    virtual ~Shader() = default;

  private:
    wgpu::RenderPipeline pipeline;
};

} // namespace Plugin::Graphic::Resource
