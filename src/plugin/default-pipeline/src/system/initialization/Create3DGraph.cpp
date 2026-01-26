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
#include "resource/pass/Shadow.hpp"
#include "system/WindowSystem.hpp"
#include "utils/EndRenderTexture.hpp"

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
            normalOutput.getClearColorCallback = [](Engine::Core &, glm::vec4 &clearColor) {
                clearColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
                return true;
            };
            GBufferPass.AddOutput(0, std::move(normalOutput));
        }
        {
            Graphic::Resource::ColorOutput albedoOutput;
            albedoOutput.textureId = DefaultPipeline::Resource::GBUFFER_PASS_OUTPUT_ALBEDO_ID;
            albedoOutput.getClearColorCallback = [](Engine::Core &, glm::vec4 &clearColor) {
                clearColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
                return true;
            };
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
        // SHADOW
        DefaultPipeline::Resource::Shadow shadowPass{};
        {
            auto ShadowShader =
                DefaultPipeline::Resource::Shadow::CreateShader(core.GetResource<Graphic::Resource::Context>());
            core.GetResource<Graphic::Resource::ShaderContainer>().Add(DefaultPipeline::Resource::SHADOW_SHADER_ID,
                                                                       std::move(ShadowShader));
            shadowPass.BindShader(DefaultPipeline::Resource::SHADOW_SHADER_NAME);
        }
        {
            Graphic::Resource::DepthOutput output;
            output.getClearDepthCallback = [](Engine::Core &, float &clearDepth) {
                clearDepth = 1.0f;
                return true;
            };
            shadowPass.AddOutput(std::move(output));
        }
        renderGraph.Add(DefaultPipeline::Resource::SHADOW_PASS_NAME, std::move(shadowPass));
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
            output.getClearColorCallback = [](Engine::Core &, glm::vec4 &clearColor) {
                clearColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
                return true;
            };
            deferredPass.AddOutput(0, std::move(output));
        }
        renderGraph.Add(DefaultPipeline::Resource::DEFERRED_PASS_NAME, std::move(deferredPass));
    }

    renderGraph.SetDependency(DefaultPipeline::Resource::GBUFFER_PASS_NAME,
                              DefaultPipeline::Resource::DEFERRED_PASS_NAME);
    renderGraph.SetDependency(DefaultPipeline::Resource::SHADOW_PASS_NAME,
                              DefaultPipeline::Resource::DEFERRED_PASS_NAME);

    return renderGraph;
}

void DefaultPipeline::System::Create3DGraph(Engine::Core &core)
{
    auto &renderPassContainer = core.GetResource<Graphic::Resource::RenderGraphContainer>();

    CreateGBufferTextures(core);
    auto renderGraph = CreateGraph(core);
    CreateDeferredTexturesBindingGroup(core);
    renderPassContainer.SetDefault(std::move(renderGraph));
}
