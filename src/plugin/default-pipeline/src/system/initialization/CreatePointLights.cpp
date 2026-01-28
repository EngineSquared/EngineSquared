#include "system/initialization/CreatePointLights.hpp"
#include "resource/BindGroupManager.hpp"
#include "resource/GPUBufferContainer.hpp"
#include "resource/buffer/PointLightsBuffer.hpp"

namespace DefaultPipeline::System {

void CreatePointLights(Engine::Core &core)
{
    auto &bufferManager = core.GetResource<Graphic::Resource::GPUBufferContainer>();

    auto pointLightsBuffer = std::make_unique<Resource::PointLightsBuffer>();
    pointLightsBuffer->Create(core);
    auto pointLightsBufferSize = pointLightsBuffer->GetBuffer().getSize();
    bufferManager.Add(Utils::POINT_LIGHTS_BUFFER_ID, std::move(pointLightsBuffer));
}
} // namespace DefaultPipeline::System
