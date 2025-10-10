#pragma once

#include "utils/webgpu.hpp"

namespace Plugin::Graphic::Resource {

class Shader {
  public:
    Shader();
    ~Shader();

  private:
    wgpu::RenderPipeline pipeline;
};

} // namespace Plugin::Graphic::Resource
