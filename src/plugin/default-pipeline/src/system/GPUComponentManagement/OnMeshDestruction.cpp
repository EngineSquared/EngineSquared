#include "system/GPUComponentManagement/OnMeshDestruction.hpp"
#include "component/GPUMesh.hpp"
#include "resource/BindGroupManager.hpp"
#include "resource/GPUBufferContainer.hpp"

void DefaultPipeline::System::OnMeshDestruction(Engine::Core &core, Engine::EntityId entityId)
{
    Engine::Entity entity{core, entityId};

    if (!entity.HasComponents<Component::GPUMesh>())
        return;

    const auto &meshComponent = entity.GetComponents<Component::GPUMesh>();

    auto &gpuBufferContainer = core.GetResource<Graphic::Resource::GPUBufferContainer>();

    if (gpuBufferContainer.Contains(meshComponent.pointBufferId))
        gpuBufferContainer.Remove(meshComponent.pointBufferId);
    if (gpuBufferContainer.Contains(meshComponent.indexBufferId))
        gpuBufferContainer.Remove(meshComponent.indexBufferId);

    entity.RemoveComponent<Component::GPUMesh>();
}
