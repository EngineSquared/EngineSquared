#include "system/commandCreation/ExecuteRenderPass.hpp"
#include "component/Camera.hpp"
#include "component/GPUCamera.hpp"
#include "resource/RenderGraphContainer.hpp"

void Graphic::System::ExecuteRenderPass(Engine::Core &core)
{
    auto &renderPassContainer = core.GetResource<Graphic::Resource::RenderGraphContainer>();

    core.GetRegistry().view<Object::Component::Camera, Graphic::Component::GPUCamera>().each([&renderPassContainer, &core](auto, const Object::Component::Camera &camera, const Graphic::Component::GPUCamera &gpuCamera) {
        renderPassContainer.Get(gpuCamera.pipeline).Execute(core);
    });
}
