#include "system/initialization/RequestCapabilities.hpp"
#include "exception/CapabilitiesRequestError.hpp"
#include "resource/Context.hpp"
#include "resource/GraphicSettings.hpp"

namespace Plugin::Graphic::System {
void RequestCapabilities(Engine::Core &core)
{
    auto &context = core.GetResource<Resource::Context>();
    auto &settings = core.GetResource<Resource::GraphicSettings>();

    if (settings.GetWindowSystem() == Resource::WindowSystem::None)
        return;
    if (context.surface->updateCapabilities(context.adapter.value()) == wgpu::Status::Error)
    {
        throw Exception::CapabilitiesRequestError("Failed to get surface capabilities");
    }
}
} // namespace Plugin::Graphic::System
