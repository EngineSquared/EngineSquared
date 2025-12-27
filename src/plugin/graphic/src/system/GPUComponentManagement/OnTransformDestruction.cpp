#include "system/GPUComponentManagement/OnTransformDestruction.hpp"
#include "component/GPUTransform.hpp"
#include "resource/BindGroupManager.hpp"
#include "resource/GPUBufferContainer.hpp"

void Graphic::System::OnTransformDestruction(Engine::Core &core, Engine::Entity entity)
{
    if (!entity.HasComponents<Component::GPUTransform>(core))
        return;

    const auto &transformComponent = entity.GetComponents<Component::GPUTransform>(core);

    auto &gpuBufferContainer = core.GetResource<Graphic::Resource::GPUBufferContainer>();
    auto &bindGroupManager = core.GetResource<Graphic::Resource::BindGroupManager>();

    if (gpuBufferContainer.Contains(transformComponent.modelMatrixBuffer))
        gpuBufferContainer.Remove(transformComponent.modelMatrixBuffer);
    if (bindGroupManager.Contains(transformComponent.bindGroup))
        bindGroupManager.Remove(transformComponent.bindGroup);

    entity.RemoveComponent<Graphic::Component::GPUTransform>(core);
}
