#include "system/preparation/UpdateGPUTransforms.hpp"
#include "component/GPUTransform.hpp"
#include "resource/GPUBufferContainer.hpp"

void DefaultPipeline::System::UpdateGPUTransforms(Engine::Core &core)
{
    auto &gpuBufferContainer = core.GetResource<Graphic::Resource::GPUBufferContainer>();
    core.GetRegistry().view<Component::GPUTransform>().each(
        [&core, &gpuBufferContainer](Component::GPUTransform &gpuTransform) {
            auto &gpuBuffer = gpuBufferContainer.Get(gpuTransform.modelMatrixBuffer);
            gpuBuffer->Update(core);
        });
}
