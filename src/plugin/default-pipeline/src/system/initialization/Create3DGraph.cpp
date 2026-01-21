#include "system/initialization/Create3DGraph.hpp"
#include "event/OnResize.hpp"
#include "resource/EventManager.hpp"
#include "resource/RenderGraphContainer.hpp"
#include "resource/pass/GBuffer.hpp"
#include "utils/DefaultRenderPass.hpp"

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

wgpu::TextureDescriptor CreateGBufferPassOutputDepthTextureDescriptor(glm::uvec2 size)
{
    wgpu::TextureDescriptor descriptor{wgpu::Default};
    descriptor.label = wgpu::StringView(DefaultPipeline::Resource::GBUFFER_PASS_OUTPUT_DEPTH);
    descriptor.size = {size.x, size.y, 1};
    descriptor.dimension = wgpu::TextureDimension::_2D;
    descriptor.format = wgpu::TextureFormat::Depth32Float;
    descriptor.usage = wgpu::TextureUsage::RenderAttachment | wgpu::TextureUsage::CopySrc | wgpu::TextureUsage::CopyDst;
    return descriptor;
}

static void CreateGBufferTextures(Engine::Core &core)
{
    // TODO: resize on window resize
    const auto &context = core.GetResource<Graphic::Resource::Context>();
    auto &textureContainer = core.GetResource<Graphic::Resource::TextureContainer>();
    auto &eventManager = core.GetResource<Event::Resource::EventManager>();

    {
        auto descriptor = CreateGBufferPassOutputNormalTextureDescriptor({800, 800});
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
        auto descriptor = CreateGBufferPassOutputAlbedoTextureDescriptor({800, 800});
        Graphic::Resource::Texture defaultTexture(context, descriptor);
        textureContainer.Add(DefaultPipeline::Resource::GBUFFER_PASS_OUTPUT_ALBEDO_ID, std::move(defaultTexture));
        eventManager.RegisterCallback<Window::Event::OnResize>([&core, &textureContainer,
                                                                &context](const Window::Event::OnResize &event) {
            auto descriptor = CreateGBufferPassOutputAlbedoTextureDescriptor({event.newSize.x, event.newSize.y});
            Graphic::Resource::Texture resizedTexture(context, descriptor);
            textureContainer.Remove(DefaultPipeline::Resource::GBUFFER_PASS_OUTPUT_NORMAL_ID);
            textureContainer.Add(DefaultPipeline::Resource::GBUFFER_PASS_OUTPUT_NORMAL_ID, std::move(resizedTexture));
        });
    }
    {
        auto descriptor = CreateGBufferPassOutputDepthTextureDescriptor({800, 800});
        Graphic::Resource::Texture defaultTexture(context, descriptor);
        textureContainer.Add(DefaultPipeline::Resource::GBUFFER_PASS_OUTPUT_DEPTH_ID, std::move(defaultTexture));
        eventManager.RegisterCallback<Window::Event::OnResize>([&core, &textureContainer,
                                                                &context](const Window::Event::OnResize &event) {
            auto descriptor = CreateGBufferPassOutputDepthTextureDescriptor({event.newSize.x, event.newSize.y});
            Graphic::Resource::Texture resizedTexture(context, descriptor);
            textureContainer.Remove(DefaultPipeline::Resource::GBUFFER_PASS_OUTPUT_NORMAL_ID);
            textureContainer.Add(DefaultPipeline::Resource::GBUFFER_PASS_OUTPUT_NORMAL_ID, std::move(resizedTexture));
        });
    }
}

static Graphic::Resource::RenderGraph CreateGraph(Engine::Core &core)
{
    Graphic::Resource::RenderGraph renderGraph{};
    {
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
    return renderGraph;
}

void DefaultPipeline::System::Create3DGraph(Engine::Core &core)
{
    auto &renderPassContainer = core.GetResource<Graphic::Resource::RenderGraphContainer>();

    CreateGBufferTextures(core);
    auto renderGraph = CreateGraph(core);
    renderPassContainer.Add(Resource::GBUFFER_PASS_ID, std::move(renderGraph));
}
