#include "Sampler.hpp"
#include <utility>

Graphic::Resource::Sampler::Sampler(const wgpu::Device &device)
{
    wgpu::SamplerDescriptor samplerDesc(wgpu::Default);

    samplerDesc.maxAnisotropy = 1;
    samplerDesc.addressModeW = wgpu::AddressMode::Repeat;
    samplerDesc.addressModeU = wgpu::AddressMode::Repeat;
    samplerDesc.addressModeV = wgpu::AddressMode::Repeat;

    _sampler = device.createSampler(samplerDesc);
}

Graphic::Resource::Sampler::Sampler(const wgpu::Device &device, const wgpu::SamplerDescriptor &samplerDesc)
    : _sampler(device.createSampler(samplerDesc))
{
}

Graphic::Resource::Sampler::~Sampler()
{
    if (_sampler != nullptr)
    {
        _sampler.release();
    }
}

Graphic::Resource::Sampler::Sampler(Sampler &&other) noexcept : _sampler(std::exchange(other._sampler, {})) {}

Graphic::Resource::Sampler &Graphic::Resource::Sampler::operator=(Sampler &&other) noexcept
{
    if (this != &other)
    {
        if (_sampler != nullptr)
        {
            _sampler.release();
        }
        _sampler = std::exchange(other._sampler, {});
    }
    return *this;
}

const wgpu::Sampler &Graphic::Resource::Sampler::GetSampler() const noexcept { return _sampler; }
