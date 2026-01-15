#include "system/GPUComponentManagement/OnMeshDestruction.hpp"
#include "component/GPUMesh.hpp"
#include "resource/BindGroupManager.hpp"
#include "resource/GPUBufferContainer.hpp"

void DefaultPipeline::System::OnMeshDestruction(Engine::Core &core, Engine::Entity entity)
{
    if (!entity.HasComponents<Component::GPUMesh>(core))
        return;

    const auto &meshComponent = entity.GetComponents<Component::GPUMesh>(core);

    auto &gpuBufferContainer = core.GetResource<Graphic::Resource::GPUBufferContainer>();

    if (gpuBufferContainer.Contains(meshComponent.pointBufferId))
        gpuBufferContainer.Remove(meshComponent.pointBufferId);
    if (gpuBufferContainer.Contains(meshComponent.indexBufferId))
        gpuBufferContainer.Remove(meshComponent.indexBufferId);

    entity.RemoveComponent<Component::GPUMesh>(core);
}
