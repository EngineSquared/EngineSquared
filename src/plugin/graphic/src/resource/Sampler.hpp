#pragma once

#include "utils/webgpu.hpp"
#include "core/Core.hpp"
#include "resource/DeviceContext.hpp"

namespace Graphic::Resource
{
    class Sampler
    {
        public:
            Sampler(Engine::Core &core, const wgpu::AddressMode &addressMode = wgpu::AddressMode::ClampToEdge) :
                _device(core.GetResource<DeviceContext>().GetDevice().value())
            {
                wgpu::SamplerDescriptor samplerDesc;

                samplerDesc.addressModeU = addressMode;
                samplerDesc.addressModeV = addressMode;
                samplerDesc.addressModeW = addressMode;
                samplerDesc.magFilter = wgpu::FilterMode::Linear;
                samplerDesc.minFilter = wgpu::FilterMode::Linear;
                samplerDesc.mipmapFilter = wgpu::MipmapFilterMode::Linear;
                samplerDesc.lodMinClamp = 0.0f;
                samplerDesc.lodMaxClamp = 1.0f;
                samplerDesc.compare = wgpu::CompareFunction::Undefined;
                samplerDesc.maxAnisotropy = 1;

                _sampler = _device.createSampler(samplerDesc);
            }
            Sampler(Engine::Core &core, const wgpu::SamplerDescriptor &samplerDesc) :
                _device(core.GetResource<DeviceContext>().GetDevice().value())
            {
                _device.createSampler(samplerDesc);
            }

            virtual ~Sampler() = default;

        private:
            wgpu::Device &_device;
            wgpu::Sampler _sampler;
    };
} // namespace Graphic::Resource
