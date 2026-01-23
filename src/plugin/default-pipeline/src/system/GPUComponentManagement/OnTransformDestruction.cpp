#include "system/GPUComponentManagement/OnTransformDestruction.hpp"
#include "component/GPUTransform.hpp"
#include "resource/BindGroupManager.hpp"
#include "resource/GPUBufferContainer.hpp"

void DefaultPipeline::System::OnTransformDestruction(Engine::Core &core, Engine::EntityId entityId)
{
    Engine::Entity entity{core, entityId};
    if (!entity.HasComponents<Component::GPUTransform>())
        return;

    const auto &transformComponent = entity.GetComponents<Component::GPUTransform>();

    auto &gpuBufferContainer = core.GetResource<Graphic::Resource::GPUBufferContainer>();
    auto &bindGroupManager = core.GetResource<Graphic::Resource::BindGroupManager>();

    if (gpuBufferContainer.Contains(transformComponent.modelMatrixBuffer))
        gpuBufferContainer.Remove(transformComponent.modelMatrixBuffer);
    if (bindGroupManager.Contains(transformComponent.bindGroup))
        bindGroupManager.Remove(transformComponent.bindGroup);

    entity.RemoveComponent<Component::GPUTransform>();
}
