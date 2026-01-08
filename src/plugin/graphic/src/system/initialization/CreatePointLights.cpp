#include "CreatePointLights.hpp"
#include "resource/BindGroupManager.hpp"
#include "resource/GPUBufferContainer.hpp"
#include "resource/PointLights.hpp"
#include "resource/buffer/PointLightsBuffer.hpp"
#include "utils/PointLight.hpp"

namespace Graphic::System {

void CreatePointLights(Engine::Core &core)
{
    auto &pointLights = core.GetResource<Graphic::Resource::PointLights>();
    pointLights.Create(core);
}

} // namespace Graphic::System
