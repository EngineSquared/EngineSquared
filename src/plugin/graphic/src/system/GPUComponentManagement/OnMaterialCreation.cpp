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
#include <filesystem>
#include <string>

void Graphic::System::OnMaterialCreation(Engine::Core &core, Engine::Entity entity)
{
    auto &gpuBufferContainer = core.GetResource<Graphic::Resource::GPUBufferContainer>();
    const auto &material = entity.GetComponents<Object::Component::Material>(core);
    auto &textureContainer = core.GetResource<Graphic::Resource::TextureContainer>();
    auto &samplerContainer = core.GetResource<Graphic::Resource::SamplerContainer>();
    const auto &context = core.GetResource<Graphic::Resource::Context>();

    Graphic::Component::GPUMaterial &GPUMaterial = entity.AddComponent<Graphic::Component::GPUMaterial>(core);
    const std::string entityString = Log::EntityToDebugString(static_cast<Engine::Entity::entity_id_type>(entity));

    entt::hashed_string textureId{material.ambientTexName.data(), material.ambientTexName.size()};
    entt::hashed_string samplerId{material.ambientTexName.data(), material.ambientTexName.size()};

    if (std::filesystem::exists(material.ambientTexName) == false)
    {
        Log::Warn(fmt::format("Material texture file not found: {}", material.ambientTexName));
        GPUMaterial.texture = entt::hashed_string{};
    }
    else
    {
        Resource::Texture texture{context, material.ambientTexName, Resource::Image(material.ambientTexName)};
        textureContainer.Add(textureId, std::move(texture));
        GPUMaterial.texture = textureId;
    }

    Resource::Sampler sampler{context.deviceContext.GetDevice().value()};
    samplerContainer.Add(samplerId, std::move(sampler));
    GPUMaterial.sampler = samplerId;

    auto materialBuffer = std::make_unique<Graphic::Resource::MaterialGPUBuffer>(entity);
    materialBuffer->Create(core);
    uint64_t materialBufferSize = materialBuffer->GetBuffer().getSize();
    const std::string_view materialBufferName = materialBuffer->GetDebugName();
    entt::hashed_string materialBufferId{materialBufferName.data(), materialBufferName.size()};
    gpuBufferContainer.Add(materialBufferId, std::move(materialBuffer));
    GPUMaterial.buffer = materialBufferId;

    auto &bindGroupManager = core.GetResource<Graphic::Resource::BindGroupManager>();
    std::string bindGroupName = "MATERIAL_BIND_GROUP_" + entityString;
    entt::hashed_string bindGroupId{bindGroupName.data(), bindGroupName.size()};
    Resource::BindGroup bindGroup(core, "DEFAULT_RENDER_PASS_SHADER", 2,
                                  {
                                      {
                                       0, Resource::BindGroup::Asset::Type::Buffer,
                                       materialBufferId, materialBufferSize,
                                       },
                                      {1, Resource::BindGroup::Asset::Type::Texture, textureId, 0},
                                      {2, Resource::BindGroup::Asset::Type::Sampler, samplerId, 0},
    });
    bindGroupManager.Add(bindGroupId, std::move(bindGroup));
    GPUMaterial.bindGroup = bindGroupId;
}
