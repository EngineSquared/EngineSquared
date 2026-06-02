#include "OnMaterialUpdate.hpp"
#include "component/GPUMaterial.hpp"
#include "resource/BindGroupManager.hpp"
#include "resource/pass/GBuffer.hpp"

void DefaultPipeline::System::OnMaterialUpdate(Engine::Core &core, Engine::EntityId entityId)
{
    Engine::Entity entity{core, entityId};
    auto &gpuBufferContainer = core.GetResource<Graphic::Resource::GPUBufferContainer>();
    auto &GPUMaterial = entity.GetComponents<Component::GPUMaterial>();
    const auto &CPUMaterial = entity.GetComponents<Object::Component::Material>();
    auto &textureContainer = core.GetResource<Graphic::Resource::TextureContainer>();
    const auto &deviceContext = core.GetResource<Graphic::Resource::DeviceContext>();
    const auto &queue = core.GetResource<Graphic::Resource::Queue>();
    const auto &materialBufferId = GPUMaterial.buffer;
    const auto &materialBufferSize = gpuBufferContainer.Get(materialBufferId).get()->GetBuffer().getSize();
    const auto &samplerId = GPUMaterial.sampler;

    entt::hashed_string textureId{CPUMaterial.diffuseTexName.data(), CPUMaterial.diffuseTexName.size()};
    if (textureContainer.Contains(textureId))
    {
        GPUMaterial.texture = textureId;
    }
    else if (std::filesystem::exists(CPUMaterial.diffuseTexName))
    {
        Graphic::Resource::Texture texture{deviceContext, queue, CPUMaterial.diffuseTexName,
                                           Graphic::Resource::Image(CPUMaterial.diffuseTexName)};
        textureContainer.Add(textureId, std::move(texture));
        GPUMaterial.texture = textureId;
    }
    else if (!CPUMaterial.diffuseTexName.empty())
    {
        Log::Warning(fmt::format("Texture '{}' not found as file or in texture container", CPUMaterial.diffuseTexName));
    }

    auto &bindGroupManager = core.GetResource<Graphic::Resource::BindGroupManager>();
    std::string bindGroupName = fmt::format("MATERIAL_BIND_GROUP_{}", entity);
    entt::hashed_string bindGroupId{bindGroupName.data(), bindGroupName.size()};

    if (bindGroupManager.Contains(bindGroupId))
        bindGroupManager.Remove(bindGroupId);

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
