#include "system/preparation/UpdateGPUMaterials.hpp"
#include "component/GPUMaterial.hpp"
#include "resource/GPUBufferContainer.hpp"

void DefaultPipeline::System::UpdateGPUMaterials(Engine::Core &core)
{
    auto &gpuBufferContainer = core.GetResource<Graphic::Resource::GPUBufferContainer>();
    core.GetRegistry().view<Component::GPUMaterial>().each(
        [&core, &gpuBufferContainer](Component::GPUMaterial &gpuMaterial) {
            auto &gpuBuffer = gpuBufferContainer.Get(gpuMaterial.buffer);
            gpuBuffer->Update(core);
        });
}
