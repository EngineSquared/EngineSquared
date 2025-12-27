#include "system/GPUComponentManagement/OnMeshCreation.hpp"
#include "component/GPUMesh.hpp"
#include "component/Mesh.hpp"
#include "resource/GPUBufferContainer.hpp"
#include "resource/buffer/IndexGPUBuffer.hpp"
#include "resource/buffer/PointGPUBuffer.hpp"
#include <string>

void Graphic::System::OnMeshCreation(Engine::Core &core, Engine::Entity entity)
{
    Graphic::Component::GPUMesh &GPUMesh = entity.AddComponent<Graphic::Component::GPUMesh>(core);
    const auto &mesh = entity.GetComponents<Object::Component::Mesh>(core);

    std::string entityString = Log::EntityToDebugString(static_cast<Engine::Entity::entity_id_type>(entity));

    auto &gpuBufferContainer = core.GetResource<Graphic::Resource::GPUBufferContainer>();

    std::string pointBufferName = "POINT_BUFFER_" + entityString;
    entt::hashed_string pointBufferUUID{pointBufferName.data(), pointBufferName.size()};
    gpuBufferContainer.Add(pointBufferUUID, std::make_unique<Graphic::Resource::PointGPUBuffer>(entity));
    gpuBufferContainer.Get(pointBufferUUID)->Create(core);
    GPUMesh.pointBufferId = pointBufferUUID;

    std::string indexBufferName = "INDEX_BUFFER_" + entityString;
    entt::hashed_string indexBufferUUID{indexBufferName.data(), indexBufferName.size()};
    gpuBufferContainer.Add(indexBufferUUID, std::make_unique<Graphic::Resource::IndexGPUBuffer>(entity));
    gpuBufferContainer.Get(indexBufferUUID)->Create(core);
    GPUMesh.indexBufferId = indexBufferUUID;
}
