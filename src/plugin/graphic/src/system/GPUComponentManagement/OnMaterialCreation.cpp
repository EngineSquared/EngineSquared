#include "system/GPUComponentManagement/OnMaterialCreation.hpp"
#include "component/GPUMaterial.hpp"
#include "component/Material.hpp"
#include "resource/BindGroup.hpp"
#include "resource/BindGroupManager.hpp"
#include "resource/GPUBufferContainer.hpp"
#include "resource/buffer/MaterialGPUBuffer.hpp"
#include <string>

void Graphic::System::OnMaterialCreation(Engine::Core &core, entt::entity e)
{
    Engine::Entity entity{e};
    Graphic::Component::GPUMaterial &GPUMaterial = entity.AddComponent<Graphic::Component::GPUMaterial>(core);
    const auto &material = entity.GetComponents<Object::Component::Material>(core);

    std::string entityString = Log::EntityToDebugString(static_cast<Engine::Entity::entity_id_type>(entity));

    auto &gpuBufferContainer = core.GetResource<Graphic::Resource::GPUBufferContainer>();

    std::unique_ptr<Graphic::Resource::MaterialGPUBuffer> materialBuffer =
        std::make_unique<Graphic::Resource::MaterialGPUBuffer>(entity);
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
    });
    bindGroupManager.Add(bindGroupId, std::move(bindGroup));
    GPUMaterial.bindGroup = bindGroupId;
}
