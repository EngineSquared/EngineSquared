#pragma once

#include "core/Core.hpp"
#include "resource/DeviceContext.hpp"
#include "utils/webgpu.hpp"

namespace Graphic::Resource {
class Sampler {
  public:
    Sampler(wgpu::Device &device);
    Sampler(wgpu::Device &device, const wgpu::SamplerDescriptor &samplerDesc);
    virtual ~Sampler();

    const wgpu::Sampler &getSampler() const noexcept;

  private:
    wgpu::Sampler _sampler;
};
} // namespace Graphic::Resource
