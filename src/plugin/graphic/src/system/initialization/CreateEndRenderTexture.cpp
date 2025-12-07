#include "CreateEndRenderTexture.hpp"
#include "exception/EndRenderTextureCreationError.hpp"
#include "resource/Context.hpp"
#include "resource/GraphicSettings.hpp"
#include "resource/TextureContainer.hpp"
#include "utils/webgpu.hpp"

namespace {
void CreatePlaceholderEndRenderTexture(Graphic::Resource::Context &context,
                                       Graphic::Resource::TextureContainer &textureContainer)
{
    wgpu::TextureDescriptor textureDesc(wgpu::Default);
    textureDesc.label = wgpu::StringView("end_render_texture");
    textureDesc.size = {.width = 1920u, .height = 1080u, .depthOrArrayLayers = 1};
    textureDesc.format = wgpu::TextureFormat::RGBA8Unorm;
    textureDesc.usage = wgpu::TextureUsage::RenderAttachment | wgpu::TextureUsage::TextureBinding |
                        wgpu::TextureUsage::CopySrc | wgpu::TextureUsage::CopyDst;
    textureDesc.viewFormats = nullptr;
    textureDesc.viewFormatCount = 0;

    Graphic::Resource::Texture texture(context, textureDesc);
    textureContainer.Add(Graphic::System::END_RENDER_TEXTURE_ID, std::move(texture));
}

void CreateSurfaceEndRenderTexture(Graphic::Resource::Context &context,
                                   Graphic::Resource::TextureContainer &textureContainer)
{
    if (!context.surface.has_value() || !context.surface->value.has_value())
    {
        throw Graphic::Exception::EndRenderTextureCreationError(
            "Surface is not created, cannot create the end render texture.");
    }

    const wgpu::Surface &surface = context.surface->value.value();
    wgpu::SurfaceTexture surfaceTexture(wgpu::Default);
    surface.getCurrentTexture(&surfaceTexture);

    if (surfaceTexture.status != wgpu::SurfaceGetCurrentTextureStatus::SuccessOptimal &&
        surfaceTexture.status != wgpu::SurfaceGetCurrentTextureStatus::SuccessSuboptimal)
    {
        throw Graphic::Exception::EndRenderTextureCreationError(fmt::format(
            "Failed to get current texture from surface, status: {}", static_cast<int>(surfaceTexture.status)));
    }

    if (surfaceTexture.texture == nullptr)
    {
        throw Graphic::Exception::EndRenderTextureCreationError("Surface texture is null");
    }

    wgpu::Texture currentTexture = surfaceTexture.texture;
    textureContainer.Add(Graphic::System::END_RENDER_TEXTURE_ID,
                         Graphic::Resource::Texture("end_render_texture", currentTexture));
}
} // namespace

void Graphic::System::CreateEndRenderTexture(Engine::Core &core)
{
    auto &context = core.GetResource<Resource::Context>();
    auto &textureContainer = core.GetResource<Resource::TextureContainer>();

    if (core.GetResource<Resource::GraphicSettings>().GetWindowSystem() == Resource::WindowSystem::None)
    {
        CreatePlaceholderEndRenderTexture(context, textureContainer);
    }
    else
    {
        CreateSurfaceEndRenderTexture(context, textureContainer);
    }
}
