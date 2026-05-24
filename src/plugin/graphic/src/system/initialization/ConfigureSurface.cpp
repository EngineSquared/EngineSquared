#include "system/initialization/ConfigureSurface.hpp"
#include "resource/Adapter.hpp"
#include "resource/DeviceContext.hpp"
#include "resource/GraphicSettings.hpp"
#include "resource/Surface.hpp"
#include "resource/Window.hpp"
#include "utils/webgpu.hpp"

void Graphic::System::ConfigureSurface(Engine::Core &core)
{
    const auto &settings = core.GetResource<Resource::GraphicSettings>();

    if (settings.GetWindowSystem() == Resource::WindowSystem::None)
        return;

    auto &deviceContext = core.GetResource<Resource::DeviceContext>();
    const auto &window = core.GetResource<Window::Resource::Window>();
    const auto surfaceSize = window.GetSize();

    wgpu::SurfaceConfiguration config(wgpu::Default);
    config.width = surfaceSize.x;
    config.height = surfaceSize.y;

    if (config.width == 0)
        config.width = 1;
    if (config.height == 0)
        config.height = 1;

    if (!core.HasResource<Resource::Surface>())
        return;
    auto &surface = core.GetResource<Resource::Surface>();

    auto &optDevice = deviceContext.GetDevice();
    if (!surface.capabilities.has_value() || !optDevice.has_value() || !surface.value.has_value())
    {
        return;
    }

    config.usage = wgpu::TextureUsage::RenderAttachment;
    config.format = surface.capabilities->formats[0];
    config.device = optDevice.value();
    config.presentMode = wgpu::PresentMode::Fifo;
    config.alphaMode = wgpu::CompositeAlphaMode::Auto;

    if (core.HasResource<Resource::Adapter>())
    {
        auto &adapter = core.GetResource<Resource::Adapter>();
        wgpu::AdapterInfo info(wgpu::Default);
        // TODO: create a class to handle RAII release properly
        if (adapter->getInfo(&info) == wgpu::Status::Success)
        {
            if (info.backendType == wgpu::BackendType::OpenGL || info.backendType == wgpu::BackendType::OpenGLES)
            {
                info.freeMembers();
                return;
            }
            info.freeMembers();
        }
    }
    surface.value->configure(config);
    surface.configured = true;
}
