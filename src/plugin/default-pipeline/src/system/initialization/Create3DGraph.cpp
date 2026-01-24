#include "system/initialization/Create3DGraph.hpp"
#include "event/OnResize.hpp"
#include "resource/Context.hpp"
#include "resource/EventManager.hpp"
#include "resource/RenderGraphContainer.hpp"
#include "resource/ShaderContainer.hpp"
#include "resource/TextureContainer.hpp"
#include "resource/Window.hpp"
#include "resource/pass/Deferred.hpp"
#include "resource/pass/GBuffer.hpp"
#include "system/WindowSystem.hpp"
#include "utils/DefaultRenderPass.hpp"
#include "utils/EndRenderTexture.hpp"

/**
 * @brief Creates a texture descriptor for the G-buffer normal output.
 *
 * @param size Texture dimensions in pixels as (width, height).
 * @return wgpu::TextureDescriptor Descriptor for a 2D RGBA16Float normal render target configured with TextureBinding,
 * RenderAttachment, CopySrc, and CopyDst usage flags.
 */
wgpu::TextureDescriptor CreateGBufferPassOutputNormalTextureDescriptor(glm::uvec2 size)
{
    wgpu::TextureDescriptor descriptor{wgpu::Default};
    descriptor.label = wgpu::StringView(DefaultPipeline::Resource::GBUFFER_PASS_OUTPUT_NORMAL);
    descriptor.size = {size.x, size.y, 1};
    descriptor.dimension = wgpu::TextureDimension::_2D;
    descriptor.format = wgpu::TextureFormat::RGBA16Float;
    descriptor.usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::RenderAttachment |
                       wgpu::TextureUsage::CopySrc | wgpu::TextureUsage::CopyDst;
    return descriptor;
}

/**
 * @brief Create a 2D BGRA8Unorm texture descriptor for the G-buffer albedo output.
 *
 * @param size Width and height of the texture in pixels.
 * @return wgpu::TextureDescriptor Descriptor configured with size (width, height, 1), 2D dimension,
 *         BGRA8Unorm format, and usage flags: TextureBinding, RenderAttachment, CopySrc, CopyDst.
 */
wgpu::TextureDescriptor CreateGBufferPassOutputAlbedoTextureDescriptor(glm::uvec2 size)
{
    wgpu::TextureDescriptor descriptor{wgpu::Default};
    descriptor.label = wgpu::StringView(DefaultPipeline::Resource::GBUFFER_PASS_OUTPUT_ALBEDO);
    descriptor.size = {size.x, size.y, 1};
    descriptor.dimension = wgpu::TextureDimension::_2D;
    descriptor.format = wgpu::TextureFormat::BGRA8Unorm;
    descriptor.usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::RenderAttachment |
                       wgpu::TextureUsage::CopySrc | wgpu::TextureUsage::CopyDst;
    return descriptor;
}

/**
 * @brief Creates a 2D depth texture descriptor for the G-buffer depth output.
 *
 * @param size Width and height of the texture in pixels (x = width, y = height).
 * @return wgpu::TextureDescriptor Descriptor configured as a 2D Depth32Float texture labeled for the G-buffer depth
 * output with usages: TextureBinding, RenderAttachment, CopySrc, and CopyDst.
 */
wgpu::TextureDescriptor CreateGBufferPassOutputDepthTextureDescriptor(glm::uvec2 size)
{
    wgpu::TextureDescriptor descriptor{wgpu::Default};
    descriptor.label = wgpu::StringView(DefaultPipeline::Resource::GBUFFER_PASS_OUTPUT_DEPTH);
    descriptor.size = {size.x, size.y, 1};
    descriptor.dimension = wgpu::TextureDimension::_2D;
    descriptor.format = wgpu::TextureFormat::Depth32Float;
    descriptor.usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::RenderAttachment |
                       wgpu::TextureUsage::CopySrc | wgpu::TextureUsage::CopyDst;
    return descriptor;
}

wgpu::TextureDescriptor CreateDeferredPassOutputTextureDescriptor(glm::uvec2 size)
{
    wgpu::TextureDescriptor descriptor{wgpu::Default};
    descriptor.label = wgpu::StringView(DefaultPipeline::Resource::DEFERRED_PASS_OUTPUT);
    descriptor.size = {size.x, size.y, 1};
    descriptor.dimension = wgpu::TextureDimension::_2D;
    descriptor.format = wgpu::TextureFormat::RGBA16Float;
    descriptor.usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::RenderAttachment |
                       wgpu::TextureUsage::CopySrc | wgpu::TextureUsage::CopyDst;
    return descriptor;
}

/**
 * @brief Creates and registers G-buffer render target textures and their resize handlers.
 *
 * Initializes default normal (RGBA16F), albedo (BGRA8Unorm), and depth (Depth32Float) textures at 800×800,
 * stores them in the engine's TextureContainer under the pipeline's G-buffer output IDs, and registers
 * Window::Event::OnResize callbacks that recreate and replace each texture when the window size changes.
 *
 * @param core Engine core used to access Graphic::Resource::Context, TextureContainer, and EventManager.
 */
static void CreateGBufferTextures(Engine::Core &core)
{
    const auto &context = core.GetResource<Graphic::Resource::Context>();
    auto &textureContainer = core.GetResource<Graphic::Resource::TextureContainer>();
    auto &eventManager = core.GetResource<Event::Resource::EventManager>();

    glm::uvec2 windowSize{Window::System::DEFAULT_WIDTH, Window::System::DEFAULT_HEIGHT};
    if (core.HasResource<Window::Resource::Window>())
    {
        windowSize = core.GetResource<Window::Resource::Window>().GetSize();
    }
    {
        auto descriptor = CreateGBufferPassOutputNormalTextureDescriptor(windowSize);
        Graphic::Resource::Texture defaultTexture(context, descriptor);
        textureContainer.Add(DefaultPipeline::Resource::GBUFFER_PASS_OUTPUT_NORMAL_ID, std::move(defaultTexture));
        eventManager.RegisterCallback<Window::Event::OnResize>([&core, &textureContainer,
                                                                &context](const Window::Event::OnResize &event) {
            auto descriptor = CreateGBufferPassOutputNormalTextureDescriptor({event.newSize.x, event.newSize.y});
            Graphic::Resource::Texture resizedTexture(context, descriptor);
            textureContainer.Remove(DefaultPipeline::Resource::GBUFFER_PASS_OUTPUT_NORMAL_ID);
            textureContainer.Add(DefaultPipeline::Resource::GBUFFER_PASS_OUTPUT_NORMAL_ID, std::move(resizedTexture));
        });
    }
    {
        auto descriptor = CreateGBufferPassOutputAlbedoTextureDescriptor(windowSize);
        Graphic::Resource::Texture defaultTexture(context, descriptor);
        textureContainer.Add(DefaultPipeline::Resource::GBUFFER_PASS_OUTPUT_ALBEDO_ID, std::move(defaultTexture));
        eventManager.RegisterCallback<Window::Event::OnResize>([&core, &textureContainer,
                                                                &context](const Window::Event::OnResize &event) {
            auto descriptor = CreateGBufferPassOutputAlbedoTextureDescriptor({event.newSize.x, event.newSize.y});
            Graphic::Resource::Texture resizedTexture(context, descriptor);
            textureContainer.Remove(DefaultPipeline::Resource::GBUFFER_PASS_OUTPUT_ALBEDO_ID);
            textureContainer.Add(DefaultPipeline::Resource::GBUFFER_PASS_OUTPUT_ALBEDO_ID, std::move(resizedTexture));
        });
    }
    {
        auto descriptor = CreateGBufferPassOutputDepthTextureDescriptor(windowSize);
        Graphic::Resource::Texture defaultTexture(context, descriptor);
        textureContainer.Add(DefaultPipeline::Resource::GBUFFER_PASS_OUTPUT_DEPTH_ID, std::move(defaultTexture));
        eventManager.RegisterCallback<Window::Event::OnResize>([&core, &textureContainer,
                                                                &context](const Window::Event::OnResize &event) {
            auto descriptor = CreateGBufferPassOutputDepthTextureDescriptor({event.newSize.x, event.newSize.y});
            Graphic::Resource::Texture resizedTexture(context, descriptor);
            textureContainer.Remove(DefaultPipeline::Resource::GBUFFER_PASS_OUTPUT_DEPTH_ID);
            textureContainer.Add(DefaultPipeline::Resource::GBUFFER_PASS_OUTPUT_DEPTH_ID, std::move(resizedTexture));
        });
    }
}

static void CreateDeferredTextures(Engine::Core &core)
{
    const auto &context = core.GetResource<Graphic::Resource::Context>();
    auto &textureContainer = core.GetResource<Graphic::Resource::TextureContainer>();
    auto &eventManager = core.GetResource<Event::Resource::EventManager>();

    glm::uvec2 windowSize{Window::System::DEFAULT_WIDTH, Window::System::DEFAULT_HEIGHT};
    if (core.HasResource<Window::Resource::Window>())
    {
        windowSize = core.GetResource<Window::Resource::Window>().GetSize();
    }
    {
        wgpu::TextureDescriptor descriptor = CreateDeferredPassOutputTextureDescriptor(windowSize);

        Graphic::Resource::Texture defaultTexture(context, descriptor);
        textureContainer.Add(DefaultPipeline::Resource::DEFERRED_PASS_OUTPUT_ID, std::move(defaultTexture));
        eventManager.RegisterCallback<Window::Event::OnResize>(
            [&core, &textureContainer, &context](const Window::Event::OnResize &event) {
                wgpu::TextureDescriptor descriptor =
                    CreateDeferredPassOutputTextureDescriptor({event.newSize.x, event.newSize.y});
                Graphic::Resource::Texture resizedTexture(context, descriptor);
                textureContainer.Remove(DefaultPipeline::Resource::DEFERRED_PASS_OUTPUT_ID);
                textureContainer.Add(DefaultPipeline::Resource::DEFERRED_PASS_OUTPUT_ID, std::move(resizedTexture));
            });
    }
}

static void CreateDeferredTexturesBindingGroup(Engine::Core &core)
{
    const auto &context = core.GetResource<Graphic::Resource::Context>();
    auto &shaderContainer = core.GetResource<Graphic::Resource::ShaderContainer>();

    Graphic::Resource::BindGroup texturesBindGroup(core, DefaultPipeline::Resource::DEFERRED_BINDGROUP_TEXTURES_NAME,
                                                   DefaultPipeline::Resource::DEFERRED_SHADER_ID, 1,
                                                   {
                                                       {0, Graphic::Resource::BindGroup::Asset::Type::Texture,
                                                        DefaultPipeline::Resource::GBUFFER_PASS_OUTPUT_NORMAL_ID, 0},
                                                       {1, Graphic::Resource::BindGroup::Asset::Type::Texture,
                                                        DefaultPipeline::Resource::GBUFFER_PASS_OUTPUT_ALBEDO_ID, 0},
                                                       {2, Graphic::Resource::BindGroup::Asset::Type::Texture,
                                                        DefaultPipeline::Resource::GBUFFER_PASS_OUTPUT_DEPTH_ID,  0}
    });
    core.GetResource<Graphic::Resource::BindGroupManager>().Add(
        DefaultPipeline::Resource::DEFERRED_BINDGROUP_TEXTURES_ID, std::move(texturesBindGroup));

    auto &eventManager = core.GetResource<Event::Resource::EventManager>();
    eventManager.RegisterCallback<Window::Event::OnResize>([&core](const Window::Event::OnResize &event) {
        auto &bindGroupManager = core.GetResource<Graphic::Resource::BindGroupManager>();
        bindGroupManager.Remove(DefaultPipeline::Resource::DEFERRED_BINDGROUP_TEXTURES_ID);

        Graphic::Resource::BindGroup texturesBindGroup(
            core, DefaultPipeline::Resource::DEFERRED_BINDGROUP_TEXTURES_NAME,
            DefaultPipeline::Resource::DEFERRED_SHADER_ID, 1,
            {
                {0, Graphic::Resource::BindGroup::Asset::Type::Texture,
                 DefaultPipeline::Resource::GBUFFER_PASS_OUTPUT_NORMAL_ID, 0},
                {1, Graphic::Resource::BindGroup::Asset::Type::Texture,
                 DefaultPipeline::Resource::GBUFFER_PASS_OUTPUT_ALBEDO_ID, 0},
                {2, Graphic::Resource::BindGroup::Asset::Type::Texture,
                 DefaultPipeline::Resource::GBUFFER_PASS_OUTPUT_DEPTH_ID,  0}
        });
        bindGroupManager.Add(DefaultPipeline::Resource::DEFERRED_BINDGROUP_TEXTURES_ID, std::move(texturesBindGroup));
    });
}

/**
 * @brief Constructs a render graph containing a configured G-buffer pass.
 *
 * Builds a RenderGraph with a single GBuffer pass that has its shader bound and three outputs:
 * normal, albedo, and depth (the depth output sets clear depth to 1.0).
 *
 * @param core Reference to the engine core used to access graphics resources and containers.
 * @return Graphic::Resource::RenderGraph The constructed render graph with the GBuffer pass added.
 */
static Graphic::Resource::RenderGraph CreateGraph(Engine::Core &core)
{
    Graphic::Resource::RenderGraph renderGraph{};
    {
        // GBUFFER
        DefaultPipeline::Resource::GBuffer GBufferPass{};

        {
            auto GBufferShader =
                DefaultPipeline::Resource::GBuffer::CreateShader(core.GetResource<Graphic::Resource::Context>());
            core.GetResource<Graphic::Resource::ShaderContainer>().Add(DefaultPipeline::Resource::GBUFFER_SHADER_ID,
                                                                       std::move(GBufferShader));
            GBufferPass.BindShader(DefaultPipeline::Resource::GBUFFER_SHADER_NAME);
        }
        {
            Graphic::Resource::ColorOutput normalOutput;
            normalOutput.textureId = DefaultPipeline::Resource::GBUFFER_PASS_OUTPUT_NORMAL_ID;
            GBufferPass.AddOutput(0, std::move(normalOutput));
        }
        {
            Graphic::Resource::ColorOutput albedoOutput;
            albedoOutput.textureId = DefaultPipeline::Resource::GBUFFER_PASS_OUTPUT_ALBEDO_ID;
            GBufferPass.AddOutput(1, std::move(albedoOutput));
        }
        {
            Graphic::Resource::DepthOutput depthOutput;
            depthOutput.textureId = DefaultPipeline::Resource::GBUFFER_PASS_OUTPUT_DEPTH_ID;
            depthOutput.getClearDepthCallback = [](Engine::Core &, float &clearDepth) {
                clearDepth = 1.0f;
                return true;
            };
            GBufferPass.AddOutput(std::move(depthOutput));
        }
        renderGraph.Add(DefaultPipeline::Resource::GBUFFER_PASS_NAME, std::move(GBufferPass));
    }
    {
        // DEFERRED
        DefaultPipeline::Resource::Deferred deferredPass{};
        {
            auto DeferredShader =
                DefaultPipeline::Resource::Deferred::CreateShader(core.GetResource<Graphic::Resource::Context>());
            core.GetResource<Graphic::Resource::ShaderContainer>().Add(DefaultPipeline::Resource::DEFERRED_SHADER_ID,
                                                                       std::move(DeferredShader));
            deferredPass.BindShader(DefaultPipeline::Resource::DEFERRED_SHADER_NAME);
        }
        {
            Graphic::Resource::ColorOutput output;
            output.textureId = Graphic::Utils::END_RENDER_TEXTURE_ID;
            deferredPass.AddOutput(0, std::move(output));
        }
        renderGraph.Add(DefaultPipeline::Resource::DEFERRED_PASS_NAME, std::move(deferredPass));
    }

    renderGraph.SetDependency(DefaultPipeline::Resource::GBUFFER_PASS_NAME,
                              DefaultPipeline::Resource::DEFERRED_PASS_NAME);
    return renderGraph;
}

/**
 * @brief Builds the G-buffer render graph, creates its associated textures, and registers the graph with the engine.
 *
 * This function ensures G-buffer textures are created (and wired to window resize events), constructs the render
 * graph containing the GBuffer pass, and stores the resulting render graph in the core's RenderGraphContainer
 * under the GBUFFER_PASS_ID.
 *
 * @param core Reference to the engine core used to access and register rendering resources (render graph container,
 *             texture container, and event manager).
 */
void DefaultPipeline::System::Create3DGraph(Engine::Core &core)
{
    auto &renderPassContainer = core.GetResource<Graphic::Resource::RenderGraphContainer>();

    CreateGBufferTextures(core);
    CreateDeferredTextures(core);
    auto renderGraph = CreateGraph(core);
    CreateDeferredTexturesBindingGroup(core);
    renderPassContainer.SetDefault(std::move(renderGraph));
}
