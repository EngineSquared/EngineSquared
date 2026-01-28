#include "resource/BindGroupManager.hpp"
#include "resource/GPUBufferContainer.hpp"
#include "resource/buffer/PointLightsBuffer.hpp"
#include "resource/pass/Deferred.hpp"
#include "system/initialization/CreatePointLights.hpp"
#include "utils/AmbientLight.hpp"
#include "utils/DirectionalLights.hpp"
#include "utils/Lights.hpp"

namespace DefaultPipeline::System {

static void CreateDirectionalLightsShadowTextures(Engine::Core &core)
{
    auto &textureContainer = core.GetResource<Graphic::Resource::TextureContainer>();
    auto &samplerContainer = core.GetResource<Graphic::Resource::SamplerContainer>();
    const auto &context = core.GetResource<Graphic::Resource::Context>();

    wgpu::TextureDescriptor textureDescriptor(wgpu::Default);
    textureDescriptor.label = wgpu::StringView("DIRECTIONAL_LIGHTS_SHADOW_TEXTURE");
    textureDescriptor.size = {8192, 8192, Utils::MAX_DIRECTIONAL_LIGHTS};
    textureDescriptor.dimension = wgpu::TextureDimension::_2D;
    textureDescriptor.format = wgpu::TextureFormat::Depth32Float;
    textureDescriptor.usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::RenderAttachment |
                              wgpu::TextureUsage::CopySrc | wgpu::TextureUsage::CopyDst;
    Graphic::Resource::Texture shadowTexture(context, textureDescriptor);
    textureContainer.Add(Utils::DIRECTIONAL_LIGHTS_SHADOW_TEXTURE_ID, std::move(shadowTexture));

    wgpu::SamplerDescriptor samplerDescriptor(wgpu::Default);
    samplerDescriptor.label = wgpu::StringView("DIRECTIONAL_LIGHTS_SHADOW_SAMPLER");
    samplerDescriptor.maxAnisotropy = 1;
    samplerDescriptor.compare = wgpu::CompareFunction::Less;
    Graphic::Resource::Sampler shadowSampler(context.deviceContext.GetDevice().value(), samplerDescriptor);
    samplerContainer.Add(Utils::DIRECTIONAL_LIGHTS_SHADOW_SAMPLER_ID, std::move(shadowSampler));
}

void CreateLights(Engine::Core &core)
{
    auto &bufferManager = core.GetResource<Graphic::Resource::GPUBufferContainer>();

    auto &ambientLightBuffer = bufferManager.Get(Utils::AMBIENT_LIGHT_BUFFER_ID);
    auto ambientLightBufferSize = ambientLightBuffer->GetBuffer().getSize();

    auto &pointLightsBuffer = bufferManager.Get(Utils::POINT_LIGHTS_BUFFER_ID);
    auto pointLightsBufferSize = pointLightsBuffer->GetBuffer().getSize();

    auto &directionalLightsBuffer = bufferManager.Get(Utils::DIRECTIONAL_LIGHTS_BUFFER_ID);
    auto directionalLightsBufferSize = directionalLightsBuffer->GetBuffer().getSize();

    CreateDirectionalLightsShadowTextures(core);

    auto &bindGroupManager = core.GetResource<Graphic::Resource::BindGroupManager>();
    Graphic::Resource::BindGroup lightsBindGroup(
        core, Utils::LIGHTS_BIND_GROUP_NAME, Resource::DEFERRED_SHADER_ID, 2,
        {
            {0, Graphic::Resource::BindGroup::Asset::Type::Buffer,  Utils::AMBIENT_LIGHT_BUFFER_ID,
             ambientLightBufferSize                                                                               },
            {1, Graphic::Resource::BindGroup::Asset::Type::Buffer,  Utils::POINT_LIGHTS_BUFFER_ID,
             pointLightsBufferSize                                                                                },
            {2, Graphic::Resource::BindGroup::Asset::Type::Buffer,  Utils::DIRECTIONAL_LIGHTS_BUFFER_ID,
             directionalLightsBufferSize                                                                          },
            {3, Graphic::Resource::BindGroup::Asset::Type::Texture, Utils::DIRECTIONAL_LIGHTS_SHADOW_TEXTURE_ID, 0},
            {4, Graphic::Resource::BindGroup::Asset::Type::Sampler, Utils::DIRECTIONAL_LIGHTS_SHADOW_SAMPLER_ID, 0}
    });
    bindGroupManager.Add(Utils::LIGHTS_BIND_GROUP_ID, std::move(lightsBindGroup));
}
} // namespace DefaultPipeline::System
