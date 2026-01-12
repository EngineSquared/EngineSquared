#include "UpdatePointLights.hpp"
#include "resource/GPUBufferContainer.hpp"
#include "utils/PointLights.hpp"

namespace DefaultPipeline::System {

void UpdatePointLights(Engine::Core &core)
{
    auto &bufferManager = core.GetResource<Graphic::Resource::GPUBufferContainer>();
    bufferManager.Get(Utils::POINT_LIGHTS_BUFFER_ID)->Update(core);
}

} // namespace DefaultPipeline::System
