#include "UpdateGPUMeshes.hpp"

#include "component/GPUMesh.hpp"
#include "component/Mesh.hpp"
#include "resource/Context.hpp"
#include "resource/GPUBufferContainer.hpp"
#include "resource/buffer/PointGPUBuffer.hpp"

namespace Graphic::System {

void UpdateGPUMeshes(Engine::Core &core)
{
    auto &registry = core.GetRegistry();
    auto &gpuBufferContainer = core.GetResource<Resource::GPUBufferContainer>();

    // Update mesh buffers for entities that have both Mesh and GPUMesh components
    auto view = registry.view<Object::Component::Mesh, Component::GPUMesh>();

    for (auto entity : view)
    {
        auto &mesh = view.get<Object::Component::Mesh>(entity);

        if (!mesh.IsDirty())
            continue;

        auto &gpuMesh = view.get<Component::GPUMesh>(entity);

        // Check if the point buffer exists and update it
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

} // namespace Graphic::System
