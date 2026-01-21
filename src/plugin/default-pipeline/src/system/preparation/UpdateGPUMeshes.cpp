#include "UpdateGPUMeshes.hpp"

#include "component/GPUMesh.hpp"
#include "component/Mesh.hpp"
#include "resource/Context.hpp"
#include "resource/GPUBufferContainer.hpp"
#include "resource/buffer/PointGPUBuffer.hpp"

namespace DefaultPipeline::System {

void UpdateGPUMeshes(Engine::Core &core)
{
    auto &registry = core.GetRegistry();
    auto &gpuBufferContainer = core.GetResource<Graphic::Resource::GPUBufferContainer>();

    auto view = registry.view<Object::Component::Mesh, Component::GPUMesh>();

    for (auto entity : view)
    {
        auto &mesh = view.get<Object::Component::Mesh>(entity);

        if (!mesh.IsDirty())
            continue;

        auto &gpuMesh = view.get<Component::GPUMesh>(entity);

        if (gpuBufferContainer.Contains(gpuMesh.pointBufferId))
        {
            auto &buffer = gpuBufferContainer.Get(gpuMesh.pointBufferId);
            if (buffer && buffer->IsCreated(core))
            {
                buffer->Update(core);
                mesh.ClearDirty();
            }
        }
    }
}

} // namespace DefaultPipeline::System
