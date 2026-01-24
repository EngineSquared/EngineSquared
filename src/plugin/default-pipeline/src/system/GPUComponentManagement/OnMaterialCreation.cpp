#include "system/GPUComponentManagement/OnMaterialCreation.hpp"
#include "Logger.hpp"
#include "component/GPUMaterial.hpp"
#include "component/Material.hpp"
#include "resource/BindGroup.hpp"
#include "resource/BindGroupManager.hpp"
#include "resource/GPUBufferContainer.hpp"
#include "resource/Sampler.hpp"
#include "resource/SamplerContainer.hpp"
#include "resource/Texture.hpp"
#include "resource/TextureContainer.hpp"
#include "resource/buffer/MaterialGPUBuffer.hpp"
#include "resource/pass/GBuffer.hpp"
#include <filesystem>
#include <string>

void DefaultPipeline::System::OnMaterialCreation(Engine::Core &core, Engine::EntityId entityId)
{
    Engine::Entity entity{core, entityId};
    auto &gpuBufferContainer = core.GetResource<Graphic::Resource::GPUBufferContainer>();
    const auto &material = entity.GetComponents<Object::Component::Material>();
    auto &textureContainer = core.GetResource<Graphic::Resource::TextureContainer>();
    auto &samplerContainer = core.GetResource<Graphic::Resource::SamplerContainer>();
    const auto &context = core.GetResource<Graphic::Resource::Context>();

    auto &GPUMaterial = entity.AddComponent<Component::GPUMaterial>();

    entt::hashed_string textureId{material.ambientTexName.data(), material.ambientTexName.size()};
    entt::hashed_string samplerId{material.ambientTexName.data(), material.ambientTexName.size()};

    if (std::filesystem::exists(material.ambientTexName))
    {
        Graphic::Resource::Texture texture{context, material.ambientTexName,
                                           Graphic::Resource::Image(material.ambientTexName)};
        textureContainer.Add(textureId, std::move(texture));
        GPUMaterial.texture = textureId;
    }
    else if (textureContainer.Contains(textureId))
    {
        GPUMaterial.texture = textureId;
    }
    else if (!material.ambientTexName.empty())
    {
        Log::Warn(fmt::format("Texture '{}' not found as file or in texture container", material.ambientTexName));
    }

    Graphic::Resource::Sampler sampler{context.deviceContext.GetDevice().value()};
    samplerContainer.Add(samplerId, std::move(sampler));
    GPUMaterial.sampler = samplerId;

    auto materialBuffer = std::make_unique<Resource::MaterialGPUBuffer>(entity);
    materialBuffer->Create(core);
    uint64_t materialBufferSize = materialBuffer->GetBuffer().getSize();
    const std::string_view materialBufferName = materialBuffer->GetDebugName();
    entt::hashed_string materialBufferId{materialBufferName.data(), materialBufferName.size()};
    gpuBufferContainer.Add(materialBufferId, std::move(materialBuffer));
    GPUMaterial.buffer = materialBufferId;

    auto &bindGroupManager = core.GetResource<Graphic::Resource::BindGroupManager>();
    std::string bindGroupName = fmt::format("MATERIAL_BIND_GROUP_{}", entity);
    entt::hashed_string bindGroupId{bindGroupName.data(), bindGroupName.size()};
    Graphic::Resource::BindGroup bindGroup(core, bindGroupName, Resource::GBUFFER_SHADER_ID, 2,
                                           {
                                               {
                                                0, Graphic::Resource::BindGroup::Asset::Type::Buffer,
                                                materialBufferId, materialBufferSize,
                                                },
                                               {1, Graphic::Resource::BindGroup::Asset::Type::Texture, textureId, 0},
                                               {2, Graphic::Resource::BindGroup::Asset::Type::Sampler, samplerId, 0},
    });
    bindGroupManager.Add(bindGroupId, std::move(bindGroup));
    GPUMaterial.bindGroup = bindGroupId;
}
