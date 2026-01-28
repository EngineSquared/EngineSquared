#include "resource/BindGroupManager.hpp"
#include "resource/GPUBufferContainer.hpp"
#include "resource/buffer/DirectionalLightsBuffer.hpp"
#include "system/initialization/CreatePointLights.hpp"

namespace DefaultPipeline::System {

void CreateDirectionalLights(Engine::Core &core)
{
    auto &bufferManager = core.GetResource<Graphic::Resource::GPUBufferContainer>();

    auto directionalLightsBuffer = std::make_unique<Resource::DirectionalLightsBuffer>();
    directionalLightsBuffer->Create(core);
    auto directionalLightsBufferSize = directionalLightsBuffer->GetBuffer().getSize();
    bufferManager.Add(Utils::DIRECTIONAL_LIGHTS_BUFFER_ID, std::move(directionalLightsBuffer));
}
} // namespace DefaultPipeline::System
