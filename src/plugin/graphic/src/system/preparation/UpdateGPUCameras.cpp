#include "system/preparation/UpdateGPUCameras.hpp"
#include "component/Camera.hpp"
#include "component/GPUCamera.hpp"
#include "component/Transform.hpp"
#include "entity/Entity.hpp"
#include "resource/BindGroupManager.hpp"
#include "resource/GPUBufferContainer.hpp"

void Graphic::System::UpdateGPUCameras(Engine::Core &core)
{
    auto &gpuBufferContainer = core.GetResource<Graphic::Resource::GPUBufferContainer>();
    core.GetRegistry().view<Object::Component::Transform, Object::Component::Camera, Graphic::Component::GPUCamera>().each(
        [&core, &gpuBufferContainer](Object::Component::Transform &transform, Object::Component::Camera &camera, Graphic::Component::GPUCamera &gpuCamera) {
            gpuCamera.Update(camera, transform);
            auto &gpuBuffer = gpuBufferContainer.Get(gpuCamera.buffer);
            gpuBuffer->Update(core);
        });
}
