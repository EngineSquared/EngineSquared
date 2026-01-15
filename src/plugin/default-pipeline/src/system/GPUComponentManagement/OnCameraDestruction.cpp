#include "system/GPUComponentManagement/OnCameraDestruction.hpp"
#include "component/GPUCamera.hpp"
#include "resource/BindGroupManager.hpp"
#include "resource/GPUBufferContainer.hpp"

void DefaultPipeline::System::OnCameraDestruction(Engine::Core &core, Engine::Entity entity)
{
    if (!entity.HasComponents<Component::GPUCamera>(core))
        return;

    const auto &cameraComponent = entity.GetComponents<Component::GPUCamera>(core);

    auto &gpuBufferContainer = core.GetResource<Graphic::Resource::GPUBufferContainer>();
    auto &bindGroupManager = core.GetResource<Graphic::Resource::BindGroupManager>();

    if (gpuBufferContainer.Contains(cameraComponent.buffer))
        gpuBufferContainer.Remove(cameraComponent.buffer);
    if (bindGroupManager.Contains(cameraComponent.bindGroup))
        bindGroupManager.Remove(cameraComponent.bindGroup);

    entity.RemoveComponent<Component::GPUCamera>(core);
}
