#include "system/GPUComponentManagement/OnDirectionalLightCreation.hpp"
#include "Logger.hpp"
#include "component/DirectionalLight.hpp"
#include "component/GPUDirectionalLight.hpp"
#include "resource/BindGroup.hpp"
#include "resource/BindGroupManager.hpp"
#include "resource/GPUBufferContainer.hpp"
#include "resource/Sampler.hpp"
#include "resource/SamplerContainer.hpp"
#include "resource/Texture.hpp"
#include "resource/TextureContainer.hpp"
#include "resource/buffer/DirectionalLightBuffer.hpp"
#include "resource/pass/GBuffer.hpp"
#include "resource/pass/Shadow.hpp"
#include "utils/DirectionalLights.hpp"
#include <filesystem>
#include <string>

void DefaultPipeline::System::OnDirectionalLightCreation(Engine::Core &core, Engine::EntityId entityId)
{
    static uint32_t lightIndex = 0;
    Engine::Entity entity{core, entityId};
    auto &gpuBufferContainer = core.GetResource<Graphic::Resource::GPUBufferContainer>();
    auto &bindGroupManager = core.GetResource<Graphic::Resource::BindGroupManager>();
    auto &textureContainer = core.GetResource<Graphic::Resource::TextureContainer>();
    auto &samplerContainer = core.GetResource<Graphic::Resource::SamplerContainer>();
    const auto &context = core.GetResource<Graphic::Resource::Context>();

    auto &GPUDirectionalLight = entity.AddComponent<Component::GPUDirectionalLight>();

    auto directionalLightBuffer = std::make_unique<Resource::DirectionalLightBuffer>(entity);
    directionalLightBuffer->Create(core);
    const std::string_view directionalLightBufferName = directionalLightBuffer->GetDebugName();
    entt::hashed_string directionalLightBufferId{directionalLightBufferName.data(), directionalLightBufferName.size()};
    gpuBufferContainer.Add(directionalLightBufferId, std::move(directionalLightBuffer));
    GPUDirectionalLight.buffer = directionalLightBufferId;

    const auto &directionalShadowsTexture = textureContainer.Get(Utils::DIRECTIONAL_LIGHTS_SHADOW_TEXTURE_ID);
    wgpu::TextureViewDescriptor shadowTextureViewDesc(wgpu::Default);
    std::string textureViewName = fmt::format("DIRECTIONAL_LIGHTS_SHADOW_TEXTURE_VIEW_{}", entity);
    shadowTextureViewDesc.label = wgpu::StringView(textureViewName.c_str());
    shadowTextureViewDesc.format = wgpu::TextureFormat::Depth32Float;
    shadowTextureViewDesc.dimension = wgpu::TextureViewDimension::_2D;
    shadowTextureViewDesc.aspect = wgpu::TextureAspect::DepthOnly;
    shadowTextureViewDesc.baseMipLevel = 0;
    shadowTextureViewDesc.mipLevelCount = 1;
    shadowTextureViewDesc.baseArrayLayer = lightIndex;
    shadowTextureViewDesc.arrayLayerCount = 1;
    shadowTextureViewDesc.usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::RenderAttachment;
    GPUDirectionalLight.shadowTextureView = directionalShadowsTexture.CreateView(shadowTextureViewDesc);

    std::string bindGroupName = fmt::format("DIRECTIONAL_LIGHT_BIND_GROUP_{}", entity);
    const uint64_t directionalLightBufferSize = Resource::DirectionalLightBuffer::DirectionalLightTransfer::GPUSize();
    entt::hashed_string bindGroupId{bindGroupName.data(), bindGroupName.size()};
    Graphic::Resource::BindGroup bindGroup(core, bindGroupName, Resource::SHADOW_SHADER_ID, 0,
                                           {
                                               {
                                                0, Graphic::Resource::BindGroup::Asset::Type::Buffer,
                                                directionalLightBufferId, directionalLightBufferSize,
                                                },
    });
    bindGroupManager.Add(bindGroupId, std::move(bindGroup));
    GPUDirectionalLight.bindGroupData = bindGroupId;
    lightIndex++;
}
