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
#include "resource/buffer/DirectionalLightGPUBuffer.hpp"
#include "resource/pass/GBuffer.hpp"
#include <filesystem>
#include <string>

void DefaultPipeline::System::OnDirectionalLightCreation(Engine::Core &core, Engine::EntityId entityId)
{
    Engine::Entity entity{core, entityId};
    auto &gpuBufferContainer = core.GetResource<Graphic::Resource::GPUBufferContainer>();
    auto &bindGroupManager = core.GetResource<Graphic::Resource::BindGroupManager>();
    auto &textureContainer = core.GetResource<Graphic::Resource::TextureContainer>();
    auto &samplerContainer = core.GetResource<Graphic::Resource::SamplerContainer>();
    const auto &context = core.GetResource<Graphic::Resource::Context>();

    auto &GPUDirectionalLight = entity.AddComponent<Component::GPUDirectionalLight>();

    auto directionalLightBuffer = std::make_unique<Resource::DirectionalLightGPUBuffer>(entity);
    directionalLightBuffer->Create(core);
    const std::string_view directionalLightBufferName = directionalLightBuffer->GetDebugName();
    entt::hashed_string directionalLightBufferId{directionalLightBufferName.data(), directionalLightBufferName.size()};
    gpuBufferContainer.Add(directionalLightBufferId, std::move(directionalLightBuffer));
    GPUDirectionalLight.buffer = directionalLightBufferId;

    // std::string bindGroupName = fmt::format("MATERIAL_BIND_GROUP_{}", entity);
    // entt::hashed_string bindGroupId{bindGroupName.data(), bindGroupName.size()};
    // Graphic::Resource::BindGroup bindGroup(core, bindGroupName, Resource::GBUFFER_SHADER_ID, 2,
    //                                        {
    //                                            {
    //                                             0, Graphic::Resource::BindGroup::Asset::Type::Buffer,
    //                                             materialBufferId, materialBufferSize,
    //                                             },
    //                                            {1, Graphic::Resource::BindGroup::Asset::Type::Texture, textureId, 0},
    //                                            {2, Graphic::Resource::BindGroup::Asset::Type::Sampler, samplerId, 0},
    // });
    // bindGroupManager.Add(bindGroupId, std::move(bindGroup));
    // GPUMaterial.bindGroup = bindGroupId;
}
