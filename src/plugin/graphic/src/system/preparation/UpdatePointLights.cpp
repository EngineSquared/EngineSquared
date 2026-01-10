#include "UpdatePointLights.hpp"
#include "resource/PointLights.hpp"

namespace Graphic::System {

void UpdatePointLights(Engine::Core &core)
{
    auto &pointLightsResource = core.GetResource<Graphic::Resource::PointLights>();
    pointLightsResource.Update(core);
}

} // namespace Graphic::System
