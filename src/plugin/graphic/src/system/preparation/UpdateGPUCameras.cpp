#include "system/preparation/UpdateGPUCameras.hpp"
#include "component/Camera.hpp"
#include "component/GPUCamera.hpp"
#include "component/Transform.hpp"
#include "entity/Entity.hpp"
#include "resource/BindGroupManager.hpp"
#include "resource/GPUBufferContainer.hpp"
#include "resource/TextureContainer.hpp"

void Graphic::System::UpdateGPUCameras(Engine::Core &core)
{
    auto &gpuBufferContainer = core.GetResource<Graphic::Resource::GPUBufferContainer>();
    const auto &textureContainer = core.GetResource<Graphic::Resource::TextureContainer>();

    core.GetRegistry()
        .view<Object::Component::Transform, Object::Component::Camera, Graphic::Component::GPUCamera>()
        .each([&core, &gpuBufferContainer, &textureContainer](Object::Component::Transform &transform,
                                                              Object::Component::Camera &camera,
                                                              Graphic::Component::GPUCamera &gpuCamera) {
            if (gpuCamera.targetTexture.value() != 0 && textureContainer.Contains(gpuCamera.targetTexture))
            {
                const auto &texture = textureContainer.Get(gpuCamera.targetTexture);
                gpuCamera.UpdateAspectRatio(texture.GetSize());
            }
            gpuCamera.Update(camera, transform);
            auto &gpuBuffer = gpuBufferContainer.Get(gpuCamera.buffer);
            gpuBuffer->Update(core);
        });
}
