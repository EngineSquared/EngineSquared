#include "system/initialization/RequestCapabilities.hpp"
#include "Logger.hpp"
#include "exception/CapabilitiesRequestError.hpp"
#include "resource/Context.hpp"
#include "resource/GraphicSettings.hpp"

namespace {

void LogSkippingSurfaceCapabilities()
{
    Log::Warn("Skipping surface capabilities request for software adapter (known Lavapipe bug).");
    Log::Warn("Some rendering features may be limited or use default values.");
}

bool HasValidSurface(const Graphic::Resource::Context &context)
{
    return context.surface.has_value() && context.surface->value.has_value();
}

bool ShouldSkipCapabilitiesRequest(const Graphic::Resource::Context &context)
{
    // Ne jamais bloquer si backend OpenGL/OpenGLES
    if (context.backendType == wgpu::BackendType::OpenGL || context.backendType == wgpu::BackendType::OpenGLES)
        return false;
    return context.isSoftwareAdapter;
}

} // namespace

namespace Graphic::System {
void RequestCapabilities(Engine::Core &core)
{
    auto &context = core.GetResource<Resource::Context>();

    if (!HasValidSurface(context))
        return;

    if (ShouldSkipCapabilitiesRequest(context))
    {
        LogSkippingSurfaceCapabilities();
        return;
    }

    if (context.surface->updateCapabilities(context.adapter.value()) == wgpu::Status::Error)
        throw Exception::CapabilitiesRequestError("Failed to get surface capabilities");
}
} // namespace Graphic::System
