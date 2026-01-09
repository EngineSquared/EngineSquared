#include "system/initialization/ConfigureSurface.hpp"
#include "Logger.hpp"
#include "resource/Context.hpp"
#include "resource/GraphicSettings.hpp"
#include "resource/Window.hpp"
#include "utils/webgpu.hpp"

namespace {

bool ShouldSkipSurfaceConfiguration(const Graphic::Resource::Context &context)
{
    // Ne jamais bloquer si backend OpenGL/OpenGLES
    if (context.backendType == wgpu::BackendType::OpenGL || context.backendType == wgpu::BackendType::OpenGLES)
        return false;
    return context.isSoftwareAdapter;
}

void LogSkippingSurfaceConfiguration()
{
    Log::Warn("Skipping surface configuration for software adapter (incompatible with Lavapipe).");
    Log::Warn("Surface operations will be unavailable. Consider using WGPU_BACKEND=gl instead.");
}

bool HasValidSurface(const Graphic::Resource::Context &context)
{
    return context.surface.has_value() && context.surface->value.has_value();
}

bool NeedsCapabilitiesUpdate(const Graphic::Resource::Context &context)
{
    return !context.surface->capabilities.has_value();
}

} // namespace

void Graphic::System::ConfigureSurface(Engine::Core &core)
{
    const auto &settings = core.GetResource<Resource::GraphicSettings>();

    if (settings.GetWindowSystem() == Resource::WindowSystem::None)
        return;

    auto &context = core.GetResource<Resource::Context>();
    const auto &window = core.GetResource<Window::Resource::Window>();

    if (!HasValidSurface(context))
        return;

    if (ShouldSkipSurfaceConfiguration(context))
    {
        LogSkippingSurfaceConfiguration();
        return;
    }

    int frameBufferSizeX;
    int frameBufferSizeY;
    glfwGetFramebufferSize(window.GetGLFWWindow(), &frameBufferSizeX, &frameBufferSizeY);

    if (NeedsCapabilitiesUpdate(context))
    {
        if (context.surface->updateCapabilities(context.adapter.value()) == wgpu::Status::Error)
            return;
    }

    if (frameBufferSizeX <= 0)
        frameBufferSizeX = 1;
    if (frameBufferSizeY <= 0)
        frameBufferSizeY = 1;

    wgpu::SurfaceConfiguration config(wgpu::Default);
    config.width = frameBufferSizeX;
    config.height = frameBufferSizeY;
    config.usage = wgpu::TextureUsage::RenderAttachment;

    if (context.surface->capabilities->formats == nullptr)
    {
        return;
    }

    config.format = context.surface->capabilities->formats[0];
    config.viewFormatCount = 0;
    config.viewFormats = nullptr;

    if (!context.deviceContext.GetDevice())
        return;

    config.device = context.deviceContext.GetDevice().value();
    config.presentMode = wgpu::PresentMode::Fifo;
    config.alphaMode = wgpu::CompositeAlphaMode::Auto;

    if (context.adapter.has_value())
    {
        wgpu::AdapterInfo info(wgpu::Default);
        if (context.adapter->getInfo(&info) == wgpu::Status::Success)
        {
            if (info.backendType == wgpu::BackendType::OpenGL || info.backendType == wgpu::BackendType::OpenGLES)
                return;
        }
    }

    context.surface->value->configure(config);
    context.surface->configured = true;
}
