#pragma once

#include "core/Core.hpp"
#include "resource/DeviceContext.hpp"
#include "utils/webgpu.hpp"

namespace Graphic::Resource {
class Sampler {
  public:
    explicit Sampler(const wgpu::Device &device);
    Sampler(const wgpu::Device &device, const wgpu::SamplerDescriptor &samplerDesc);

    virtual ~Sampler();

    Sampler(const Sampler &) = delete;
    Sampler &operator=(const Sampler &) = delete;

    Sampler(Sampler &&other) noexcept;
    Sampler &operator=(Sampler &&other) noexcept;

    const wgpu::Sampler &getSampler() const noexcept;

  private:
    wgpu::Sampler _sampler;
};
} // namespace Graphic::Resource
