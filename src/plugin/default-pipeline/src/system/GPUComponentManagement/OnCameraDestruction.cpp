#include "system/GPUComponentManagement/OnCameraDestruction.hpp"
#include "component/GPUCamera.hpp"
#include "resource/BindGroupManager.hpp"
#include "resource/GPUBufferContainer.hpp"

void DefaultPipeline::System::OnCameraDestruction(Engine::Core &core, Engine::EntityId entityId)
{
    Engine::Entity entity{core, entityId};

    if (!entity.HasComponents<Component::GPUCamera>())
        return;

    const auto &cameraComponent = entity.GetComponents<Component::GPUCamera>();

    auto &gpuBufferContainer = core.GetResource<Graphic::Resource::GPUBufferContainer>();
    auto &bindGroupManager = core.GetResource<Graphic::Resource::BindGroupManager>();

    if (gpuBufferContainer.Contains(cameraComponent.buffer))
        gpuBufferContainer.Remove(cameraComponent.buffer);
    if (bindGroupManager.Contains(cameraComponent.bindGroup))
        bindGroupManager.Remove(cameraComponent.bindGroup);

    entity.RemoveComponent<Component::GPUCamera>();
}
