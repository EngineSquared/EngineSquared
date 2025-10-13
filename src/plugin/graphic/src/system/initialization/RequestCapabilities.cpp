#include "system/initialization/RequestCapabilities.hpp"
#include "resource/Context.hpp"
#include "resource/GraphicSettings.hpp"
#include "exception/CapabilitiesRequestError.hpp"

namespace Plugin::Graphic::System {
void RequestCapabilities(Engine::Core &core)
{
    auto &context = core.GetResource<Resource::Context>();
    auto &settings = core.GetResource<Resource::GraphicSettings>();

    wgpu::SurfaceCapabilities capabilities(wgpu::Default);

    if (settings.GetWindowSystem() == Resource::WindowSystem::None) {
        return;
    }
    if (context.surface->getCapabilities(context.adapter.value(), capabilities) == wgpu::Status::Error) {
        throw Exception::CapabilitiesRequestError("Failed to get surface capabilities");
    }
    context.surface->capabilities = capabilities;
}
} // namespace Plugin::Graphic::System
