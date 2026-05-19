#include "system/initialization/RequestCapabilities.hpp"
#include "exception/CapabilitiesRequestError.hpp"
#include "resource/Adapter.hpp"
#include "resource/Context.hpp"
#include "resource/GraphicSettings.hpp"

namespace Graphic::System {
void RequestCapabilities(Engine::Core &core)
{
    auto &context = core.GetResource<Resource::Context>();
    auto &adapter = core.GetResource<Resource::Adapter>();

    if (!core.HasResource<Resource::Surface>())
        return;
    auto &surface = core.GetResource<Resource::Surface>();

    if (surface.updateCapabilities(*adapter) == wgpu::Status::Error)
    {
        throw Exception::CapabilitiesRequestError("Failed to get surface capabilities");
    }
}
} // namespace Graphic::System
