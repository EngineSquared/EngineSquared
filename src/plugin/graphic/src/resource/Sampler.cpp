#include "Sampler.hpp"
#include <utility>

Graphic::Resource::Sampler::Sampler(const wgpu::Device &device)
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

Graphic::Resource::Sampler::Sampler(const wgpu::Device &device, const wgpu::SamplerDescriptor &samplerDesc)
 : _sampler(device.createSampler(samplerDesc))
{
}

Graphic::Resource::Sampler::~Sampler() { _sampler.release(); }

Graphic::Resource::Sampler::Sampler(Sampler &&other) noexcept
    : _sampler(std::exchange(other._sampler, {}))
{
}

Graphic::Resource::Sampler &Graphic::Resource::Sampler::operator=(Sampler &&other) noexcept
{
    if (this != &other) {
        _sampler.release();
        _sampler = std::exchange(other._sampler, {});
    }
    return *this;
}

const wgpu::Sampler &Graphic::Resource::Sampler::getSampler() const noexcept
{
    return _sampler;
}
