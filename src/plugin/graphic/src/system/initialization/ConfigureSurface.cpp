#include "system/initialization/ConfigureSurface.hpp"
#include "resource/Context.hpp"
#include "resource/GraphicSettings.hpp"
#include "resource/Window.hpp"
#include "utils/webgpu.hpp"

void Plugin::Graphic::System::ConfigureSurface(Engine::Core &core)
{
    const auto &settings = core.GetResource<Resource::GraphicSettings>();

    if (settings.GetWindowSystem() == Resource::WindowSystem::None)
        return;

    auto &context = core.GetResource<Resource::Context>();
    const auto &window = core.GetResource<Plugin::Window::Resource::Window>();

    int frameBufferSizeX;
    int frameBufferSizeY;
    glfwGetFramebufferSize(window.GetGLFWWindow(), &frameBufferSizeX, &frameBufferSizeY);

    wgpu::SurfaceConfiguration config(wgpu::Default);
    config.width = frameBufferSizeX;
    config.height = frameBufferSizeY;
    config.usage = wgpu::TextureUsage::RenderAttachment;
    config.format = context.surface->capabilities->formats[0];
    config.viewFormatCount = 0;
    config.viewFormats = nullptr;
    config.device = context.deviceContext.GetDevice().value();
    config.presentMode = wgpu::PresentMode::Fifo;
    config.alphaMode = wgpu::CompositeAlphaMode::Auto;

    context.surface->value->configure(config);
}
