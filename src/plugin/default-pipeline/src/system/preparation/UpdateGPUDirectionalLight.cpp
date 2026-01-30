#include "system/preparation/UpdateGPUDirectionalLight.hpp"
#include "component/DirectionalLight.hpp"
#include "component/GPUDirectionalLight.hpp"
#include "component/Transform.hpp"
#include "entity/Entity.hpp"
#include "resource/BindGroupManager.hpp"
#include "resource/GPUBufferContainer.hpp"
#include "resource/TextureContainer.hpp"

void DefaultPipeline::System::UpdateGPUDirectionalLight(Engine::Core &core)
{
    auto &gpuBufferContainer = core.GetResource<Graphic::Resource::GPUBufferContainer>();
    const auto &textureContainer = core.GetResource<Graphic::Resource::TextureContainer>();

    core.GetRegistry()
        .view<Object::Component::Transform, Object::Component::DirectionalLight, Component::GPUDirectionalLight>()
        .each([&core, &gpuBufferContainer, &textureContainer](Object::Component::Transform &transform,
                                                              Object::Component::DirectionalLight &directionalLight,
                                                              Component::GPUDirectionalLight &gpuDirectionalLight) {
            gpuDirectionalLight.Update(directionalLight, transform);
            auto &gpuBuffer = gpuBufferContainer.Get(gpuDirectionalLight.buffer);
            gpuBuffer->Update(core);
        });
}
