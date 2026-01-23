#include "system/GPUComponentManagement/OnMeshCreation.hpp"
#include "component/GPUMesh.hpp"
#include "component/Mesh.hpp"
#include "resource/GPUBufferContainer.hpp"
#include "resource/buffer/IndexGPUBuffer.hpp"
#include "resource/buffer/PointGPUBuffer.hpp"
#include <string>

void DefaultPipeline::System::OnMeshCreation(Engine::Core &core, Engine::EntityId entityId)
{
    Engine::Entity entity{core, entityId};
    const auto &mesh = entity.GetComponents<Object::Component::Mesh>();
    auto &GPUMesh = entity.AddComponent<Component::GPUMesh>();
    auto &gpuBufferContainer = core.GetResource<Graphic::Resource::GPUBufferContainer>();

    std::string pointBufferName = fmt::format("POINT_BUFFER_{}", entity);
    entt::hashed_string pointBufferUUID{pointBufferName.data(), pointBufferName.size()};
    gpuBufferContainer.Add(pointBufferUUID, std::make_unique<Resource::PointGPUBuffer>(entity));
    gpuBufferContainer.Get(pointBufferUUID)->Create(core);
    GPUMesh.pointBufferId = pointBufferUUID;

    std::string indexBufferName = fmt::format("INDEX_BUFFER_{}", entity);
    entt::hashed_string indexBufferUUID{indexBufferName.data(), indexBufferName.size()};
    gpuBufferContainer.Add(indexBufferUUID, std::make_unique<Resource::IndexGPUBuffer>(entity));
    gpuBufferContainer.Get(indexBufferUUID)->Create(core);
    GPUMesh.indexBufferId = indexBufferUUID;
}
