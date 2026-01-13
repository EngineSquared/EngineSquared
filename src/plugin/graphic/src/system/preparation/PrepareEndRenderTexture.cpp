#include "system/preparation/PrepareEndRenderTexture.hpp"
#include "exception/EndRenderTextureCreationError.hpp"
#include "resource/Context.hpp"
#include "resource/GraphicSettings.hpp"
#include "resource/TextureContainer.hpp"
#include "utils/webgpu.hpp"
#include <glm/vec2.hpp>
#include "utils/EndRenderTexture.hpp"

static void EnsurePlaceholderEndRenderTexture(Graphic::Resource::Context &context,
                                              Graphic::Resource::TextureContainer &textureContainer)
{
    wgpu::TextureDescriptor textureDesc(wgpu::Default);
    std::string_view name(Graphic::Utils::END_RENDER_TEXTURE_ID.data(), Graphic::Utils::END_RENDER_TEXTURE_ID.size());
    textureDesc.label = wgpu::StringView(name);
    textureDesc.size = {.width = 1920u, .height = 1080u, .depthOrArrayLayers = 1};
    textureDesc.format = wgpu::TextureFormat::BGRA8UnormSrgb;
    textureDesc.usage = wgpu::TextureUsage::RenderAttachment | wgpu::TextureUsage::TextureBinding |
                        wgpu::TextureUsage::CopySrc | wgpu::TextureUsage::CopyDst;
    textureDesc.viewFormats = nullptr;
    textureDesc.viewFormatCount = 0;

    Graphic::Resource::Texture texture(context, textureDesc);

    if (textureContainer.Contains(Graphic::Utils::END_RENDER_TEXTURE_ID))
    {
        auto &existingTexture = textureContainer.Get(Graphic::Utils::END_RENDER_TEXTURE_ID);
        std::swap(existingTexture, texture);
    }
    else
    {
        textureContainer.Add(Graphic::Utils::END_RENDER_TEXTURE_ID, std::move(texture));
    }
}

static void EnsureSurfaceEndRenderTexture(Graphic::Resource::Context &context,
                                          Graphic::Resource::TextureContainer &textureContainer)
{
    if (!context.surface.has_value() || !context.surface->value.has_value())
    {
        throw Graphic::Exception::EndRenderTextureCreationError(
            "Surface is not created, cannot prepare the end render texture.");
    }

    if (!context.surface->configured)
    {
        EnsurePlaceholderEndRenderTexture(context, textureContainer);
        return;
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
    Graphic::Resource::Texture newTexture("end_render_texture", currentTexture, false);

    if (textureContainer.Contains(Graphic::Utils::END_RENDER_TEXTURE_ID))
    {
        auto &texture = textureContainer.Get(Graphic::Utils::END_RENDER_TEXTURE_ID);
        std::swap(texture, newTexture);
    }
    else
    {
        textureContainer.Add(Graphic::Utils::END_RENDER_TEXTURE_ID, std::move(newTexture));
    }
}

static void EnsureDepthTexture(Engine::Core &core, const glm::uvec2 &requiredSize)
{
    auto &context = core.GetResource<Graphic::Resource::Context>();
    auto &textureContainer = core.GetResource<Graphic::Resource::TextureContainer>();

    if (textureContainer.Contains(Graphic::System::END_DEPTH_RENDER_TEXTURE_ID))
    {
        const auto &existingDepth = textureContainer.Get(Graphic::System::END_DEPTH_RENDER_TEXTURE_ID);
        if (existingDepth.GetSize() == requiredSize)
        {
            return;
        }
        textureContainer.Remove(Graphic::System::END_DEPTH_RENDER_TEXTURE_ID);
    }

    wgpu::TextureDescriptor textureDesc(wgpu::Default);
    std::string_view name(Graphic::System::END_DEPTH_RENDER_TEXTURE_ID.data(),
                          Graphic::System::END_DEPTH_RENDER_TEXTURE_ID.size());
    textureDesc.label = wgpu::StringView(name);
    textureDesc.size = {requiredSize.x, requiredSize.y, 1};
    textureDesc.dimension = wgpu::TextureDimension::_2D;
    textureDesc.mipLevelCount = 1;
    textureDesc.sampleCount = 1;
    textureDesc.format = wgpu::TextureFormat::Depth24Plus;
    textureDesc.usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::RenderAttachment |
                        wgpu::TextureUsage::CopySrc | wgpu::TextureUsage::CopyDst;
    textureDesc.viewFormats = nullptr;
    textureDesc.viewFormatCount = 0;

    Graphic::Resource::Texture depthTexture(context, textureDesc);
    textureContainer.Add(Graphic::System::END_DEPTH_RENDER_TEXTURE_ID, std::move(depthTexture));
}

void Graphic::System::PrepareEndRenderTexture(Engine::Core &core)
{
    auto &context = core.GetResource<Resource::Context>();
    auto &textureContainer = core.GetResource<Resource::TextureContainer>();

    if (core.GetResource<Resource::GraphicSettings>().GetWindowSystem() == Resource::WindowSystem::None)
    {
        EnsurePlaceholderEndRenderTexture(context, textureContainer);
    }
    else
    {
        EnsureSurfaceEndRenderTexture(context, textureContainer);
    }

    const auto &endRenderTexture = textureContainer.Get(Utils::END_RENDER_TEXTURE_ID);
    EnsureDepthTexture(core, endRenderTexture.GetSize());
}
