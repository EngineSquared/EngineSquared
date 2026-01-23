#include "system/GPUComponentManagement/OnMaterialDestruction.hpp"
#include "component/GPUMaterial.hpp"
#include "resource/BindGroupManager.hpp"
#include "resource/GPUBufferContainer.hpp"

void DefaultPipeline::System::OnMaterialDestruction(Engine::Core &core, Engine::EntityId entityId)
{
    Engine::Entity entity{core, entityId};

    if (!entity.HasComponents<Component::GPUMaterial>())
        return;

    const auto &materialComponent = entity.GetComponents<Component::GPUMaterial>();

    auto &gpuBufferContainer = core.GetResource<Graphic::Resource::GPUBufferContainer>();
    auto &bindGroupManager = core.GetResource<Graphic::Resource::BindGroupManager>();

    if (gpuBufferContainer.Contains(materialComponent.buffer))
        gpuBufferContainer.Remove(materialComponent.buffer);
    if (bindGroupManager.Contains(materialComponent.bindGroup))
        bindGroupManager.Remove(materialComponent.bindGroup);

    entity.RemoveComponent<Component::GPUMaterial>();
}
