#include "system/preparation/RecreateGPUCameras.hpp"
#include "component/Camera.hpp"
#include "component/GPUCamera.hpp"
#include "entity/Entity.hpp"
#include "resource/BindGroupManager.hpp"
#include "resource/GPUBufferContainer.hpp"
#include "system/GPUComponentManagement/OnCameraCreation.hpp"

void Graphic::System::RecreateGPUCameras(Engine::Core &core)
{
    core.GetRegistry().view<Object::Component::Camera, Graphic::Component::GPUCamera>().each(
        [&](auto e, const Object::Component::Camera &camera, Graphic::Component::GPUCamera &gpuCamera) {
            Engine::Entity entity{e};
            auto &bindGroupManager = core.GetResource<Graphic::Resource::BindGroupManager>();
            bindGroupManager.Remove(gpuCamera.bindGroup);
            auto &gpuBufferContainer = core.GetResource<Graphic::Resource::GPUBufferContainer>();
            gpuBufferContainer.Remove(gpuCamera.buffer);
            entity.RemoveComponent<Graphic::Component::GPUCamera>(core);
            Graphic::System::OnCameraCreation(core, entity);
        });
}
