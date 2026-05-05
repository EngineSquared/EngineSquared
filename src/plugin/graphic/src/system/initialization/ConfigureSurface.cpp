#include "system/initialization/ConfigureSurface.hpp"
#include "resource/Context.hpp"
#include "resource/GraphicSettings.hpp"
#include "resource/Window.hpp"
#include "utils/webgpu.hpp"

void Graphic::System::ConfigureSurface(Engine::Core &core)
{
    const auto &settings = core.GetResource<Resource::GraphicSettings>();

    if (settings.GetWindowSystem() == Resource::WindowSystem::None)
        return;

    auto &context = core.GetResource<Resource::Context>();
    const auto &window = core.GetResource<Window::Resource::Window>();
    // const auto surfaceSize = window.GetSize();
    int frameBufferSizeX, frameBufferSizeY;
    glfwGetFramebufferSize(window.GetGLFWWindow(), &frameBufferSizeX, &frameBufferSizeY);

    wgpu::SurfaceConfiguration config(wgpu::Default);
    config.width = frameBufferSizeX;
    config.height = frameBufferSizeY;

    if (config.width == 0)
        config.width = 1;
    if (config.height == 0)
        config.height = 1;

    auto &optDevice = context.deviceContext.GetDevice();
    if (!context.surface.has_value() || !context.surface.value().capabilities.has_value() || !optDevice.has_value() ||
        !context.surface.value().value.has_value())
    {
        return;
    }

    config.usage = wgpu::TextureUsage::RenderAttachment;
    config.format = context.surface->capabilities->formats[0];
    config.device = optDevice.value();
    config.presentMode = wgpu::PresentMode::Fifo;
    config.alphaMode = wgpu::CompositeAlphaMode::Auto;

    if (context.adapter.has_value())
    {
        wgpu::AdapterInfo info(wgpu::Default);
        // TODO: create a class to handle RAII release properly
        if (context.adapter->getInfo(&info) == wgpu::Status::Success)
        {
            if (info.backendType == wgpu::BackendType::OpenGL || info.backendType == wgpu::BackendType::OpenGLES)
            {
                info.freeMembers();
                return;
            }
            info.freeMembers();
        }
    }

    context.surface->value->configure(config);
    context.surface->configured = true;
}
