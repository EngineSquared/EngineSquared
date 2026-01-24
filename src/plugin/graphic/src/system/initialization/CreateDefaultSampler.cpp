#include "system/initialization/CreateDefaultSampler.hpp"
#include "core/Core.hpp"
#include "resource/Context.hpp"
#include "resource/Sampler.hpp"
#include "resource/SamplerContainer.hpp"
#include "utils/DefaultSampler.hpp"
#include "utils/webgpu.hpp"
#include <utility>

namespace Graphic::System {
void CreateDefaultSampler(Engine::Core &core)
{
    const auto &context = core.GetResource<Resource::Context>();
    Resource::SamplerContainer &samplerContainer = core.GetResource<Resource::SamplerContainer>();

    wgpu::SamplerDescriptor samplerDesc(wgpu::Default);
    samplerDesc.maxAnisotropy = 1;
    samplerDesc.magFilter = wgpu::FilterMode::Linear;
    samplerDesc.minFilter = wgpu::FilterMode::Linear;
    samplerDesc.mipmapFilter = wgpu::MipmapFilterMode::Linear;
    samplerDesc.addressModeU = wgpu::AddressMode::ClampToEdge;
    samplerDesc.addressModeV = wgpu::AddressMode::ClampToEdge;
    samplerDesc.addressModeW = wgpu::AddressMode::ClampToEdge;

    Resource::Sampler defaultSampler(context.deviceContext.GetDevice().value(), samplerDesc);

    samplerContainer.Add(Utils::DEFAULT_SAMPLER_ID, std::move(defaultSampler));
}
} // namespace Graphic::System
