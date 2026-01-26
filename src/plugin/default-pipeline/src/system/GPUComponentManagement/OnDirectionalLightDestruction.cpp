#include "system/GPUComponentManagement/OnDirectionalLightDestruction.hpp"
#include "component/GPUDirectionalLight.hpp"
#include "resource/BindGroupManager.hpp"
#include "resource/GPUBufferContainer.hpp"

void DefaultPipeline::System::OnDirectionalLightDestruction(Engine::Core &core, Engine::EntityId entityId)
{
    Engine::Entity entity{core, entityId};

    if (!entity.HasComponents<Component::GPUDirectionalLight>())
        return;

    const auto &directionalLightComponent = entity.GetComponents<Component::GPUDirectionalLight>();

    auto &gpuBufferContainer = core.GetResource<Graphic::Resource::GPUBufferContainer>();
    auto &bindGroupManager = core.GetResource<Graphic::Resource::BindGroupManager>();

    if (gpuBufferContainer.Contains(directionalLightComponent.buffer))
        gpuBufferContainer.Remove(directionalLightComponent.buffer);
    if (bindGroupManager.Contains(directionalLightComponent.bindGroupData))
        bindGroupManager.Remove(directionalLightComponent.bindGroupData);

    entity.RemoveComponent<Component::GPUDirectionalLight>();
}
