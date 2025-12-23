#include "system/preparation/UpdateGPUTransforms.hpp"
#include "component/GPUTransform.hpp"
#include "resource/GPUBufferContainer.hpp"

void Graphic::System::UpdateGPUTransforms(Engine::Core &core)
{
    auto &gpuBufferContainer = core.GetResource<Graphic::Resource::GPUBufferContainer>();
    core.GetRegistry().view<Graphic::Component::GPUTransform>().each(
        [&core, &gpuBufferContainer](Graphic::Component::GPUTransform &gpuTransform) {
            auto &gpuBuffer = gpuBufferContainer.Get(gpuTransform.modelMatrixBuffer);
            gpuBuffer->Update(core);
        });
}
