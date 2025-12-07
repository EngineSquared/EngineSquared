#include "CreateEndRenderTexture.hpp"
#include "exception/EndRenderTextureCreationError.hpp"
#include "resource/Context.hpp"
#include "resource/GraphicSettings.hpp"
#include "resource/TextureContainer.hpp"
#include "utils/webgpu.hpp"

void Graphic::System::CreateEndRenderTexture(Engine::Core &core)
{
    auto &context = core.GetResource<Resource::Context>();
    auto &textureContainer = core.GetResource<Resource::TextureContainer>();
    entt::hashed_string textureId = "end_render_texture";

    if (core.GetResource<Resource::GraphicSettings>().GetWindowSystem() == Resource::WindowSystem::None)
    {
        // We create a placeholder texture when there's no window system
        wgpu::TextureDescriptor textureDesc(wgpu::Default);
        textureDesc.label = wgpu::StringView("end_render_texture");
        textureDesc.size = {.width = 1920u, .height = 1080u, .depthOrArrayLayers = 1};
        textureDesc.dimension = wgpu::TextureDimension::_2D;
        textureDesc.mipLevelCount = 1;
        textureDesc.sampleCount = 1;
        textureDesc.format = wgpu::TextureFormat::RGBA8Unorm;
        textureDesc.usage = wgpu::TextureUsage::RenderAttachment | wgpu::TextureUsage::TextureBinding |
                            wgpu::TextureUsage::CopySrc | wgpu::TextureUsage::CopyDst;
        textureDesc.viewFormats = nullptr;
        textureDesc.viewFormatCount = 0;

        Resource::Texture texture(context, textureDesc);
        textureContainer.Add(textureId, std::move(texture));
        if (context.surface.has_value())
        {
            context.surface->currentTextureId = textureId;
        }
        return;
    }

    if (!context.surface.has_value() || !context.surface->value.has_value())
    {
        throw Exception::EndRenderTextureCreationError("Surface is not created, cannot create the end render texture.");
    }

    const wgpu::Surface &surface = context.surface->value.value();
    wgpu::SurfaceTexture surfaceTexture(wgpu::Default);
    surface.getCurrentTexture(&surfaceTexture);

    if (surfaceTexture.status != wgpu::SurfaceGetCurrentTextureStatus::SuccessOptimal &&
        surfaceTexture.status != wgpu::SurfaceGetCurrentTextureStatus::SuccessSuboptimal)
    {
        throw Exception::EndRenderTextureCreationError(fmt::format(
            "Failed to get current texture from surface, status: {}", static_cast<int>(surfaceTexture.status)));
    }

    if (surfaceTexture.texture == nullptr)
    {
        throw Exception::EndRenderTextureCreationError("Surface texture is null");
    }

    wgpu::Texture currentTexture = surfaceTexture.texture;
    textureContainer.Add(textureId, Resource::Texture("end_render_texture", currentTexture));
    context.surface->currentTextureId = textureId;
}
