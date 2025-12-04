#include "CreateRenderSurface.hpp"
#include "exception/RenderSurfaceCreationError.hpp"
#include "resource/Context.hpp"
#include "resource/GraphicSettings.hpp"
#include "utils/webgpu.hpp"

void Graphic::System::CreateRenderSurface(Engine::Core &core)
{
    if (core.GetResource<Resource::GraphicSettings>().GetWindowSystem() == Resource::WindowSystem::None)
    {
        return;
    }

    auto &context = core.GetResource<Resource::Context>();

    if (!context.surface.has_value() || !context.surface->value.has_value())
    {
        throw Exception::RenderSurfaceCreationError("Surface is not created, cannot create render surface");
    }

    const wgpu::Surface &surface = context.surface->value.value();
    wgpu::SurfaceTexture surfaceTexture(wgpu::Default);
    surface.getCurrentTexture(&surfaceTexture);

    if (surfaceTexture.status != wgpu::SurfaceGetCurrentTextureStatus::SuccessOptimal &&
        surfaceTexture.status != wgpu::SurfaceGetCurrentTextureStatus::SuccessSuboptimal)
    {
        throw Exception::RenderSurfaceCreationError(fmt::format(
            "Failed to get current texture from surface, status: {}", static_cast<int>(surfaceTexture.status)));
    }

    if (surfaceTexture.texture == nullptr)
    {
        throw Exception::RenderSurfaceCreationError("Surface texture is null");
    }

    wgpu::Texture currentTexture = surfaceTexture.texture;
    wgpu::TextureView textureView = currentTexture.createView();

    if (textureView == nullptr)
    {
        throw Exception::RenderSurfaceCreationError("Failed to create texture view from surface texture");
    }

    context.surface->currentTexture = currentTexture;
    context.surface->currentTextureView = textureView;
}
