#pragma once

#include "core/Core.hpp"
#include "resource/DeviceContext.hpp"
#include "utils/webgpu.hpp"

namespace Graphic::Resource {
class Sampler {
  public:
    Sampler(wgpu::Device &device)
    {
        wgpu::SamplerDescriptor samplerDesc(wgpu::Default);

        samplerDesc.addressModeU = wgpu::AddressMode::ClampToEdge;
        samplerDesc.addressModeV = wgpu::AddressMode::ClampToEdge;
        samplerDesc.addressModeW = wgpu::AddressMode::ClampToEdge;
        samplerDesc.magFilter = wgpu::FilterMode::Linear;
        samplerDesc.minFilter = wgpu::FilterMode::Linear;
        samplerDesc.mipmapFilter = wgpu::MipmapFilterMode::Linear;
        samplerDesc.lodMinClamp = 0.0f;
        samplerDesc.lodMaxClamp = 1.0f;
        samplerDesc.compare = wgpu::CompareFunction::Undefined;
        samplerDesc.maxAnisotropy = 1;

        _sampler = device.createSampler(samplerDesc);
    }

    Sampler(wgpu::Device &device, const wgpu::SamplerDescriptor &samplerDesc)
    {
        _sampler = device.createSampler(samplerDesc);
    }

    virtual ~Sampler() { _sampler.release(); }

    const wgpu::Sampler &getSampler() const noexcept { return _sampler; }

  private:
    wgpu::Sampler _sampler;
};
} // namespace Graphic::Resource
