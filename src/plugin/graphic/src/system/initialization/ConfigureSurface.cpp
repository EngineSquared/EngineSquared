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
    const auto surfaceSize = window.GetSize();

    wgpu::SurfaceConfiguration config(wgpu::Default);
    config.width = surfaceSize.x;
    config.height = surfaceSize.y;
    config.usage = wgpu::TextureUsage::RenderAttachment;
    config.format = context.surface->capabilities->formats[0];
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
