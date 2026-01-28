#include "UpdateDirectionalLights.hpp"
#include "resource/GPUBufferContainer.hpp"
#include "utils/DirectionalLights.hpp"

namespace DefaultPipeline::System {

void UpdateDirectionalLights(Engine::Core &core)
{
    auto &bufferManager = core.GetResource<Graphic::Resource::GPUBufferContainer>();
    bufferManager.Get(Utils::DIRECTIONAL_LIGHTS_BUFFER_ID)->Update(core);
}

} // namespace DefaultPipeline::System
