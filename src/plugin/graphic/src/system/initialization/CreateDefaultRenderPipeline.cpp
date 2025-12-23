#include "system/initialization/CreateDefaultRenderPipeline.hpp"
#include "resource/RenderGraph.hpp"
#include "resource/RenderGraphContainer.hpp"
#include "resource/ShaderContainer.hpp"
#include "system/preparation/CreateEndRenderTexture.hpp"
#include "resource/ARenderPass.hpp"
#include "resource/SingleExecutionRenderPass.hpp"
#include "resource/Window.hpp"
#include "utils/DefaultPipeline.hpp"
#include <GLFW/glfw3.h>

static void CreateEndDepthRenderTexture(Engine::Core &core, const std::string &textureName)
{
    Graphic::Resource::Context &context = core.GetResource<Graphic::Resource::Context>();
    Window::Resource::Window &window = core.GetResource<Window::Resource::Window>();

    int frameBufferSizeX, frameBufferSizeY;
    glfwGetFramebufferSize(window.GetGLFWWindow(), &frameBufferSizeX, &frameBufferSizeY);

    wgpu::TextureDescriptor textureDesc(wgpu::Default);
    textureDesc.label = wgpu::StringView(textureName);
    textureDesc.size = {static_cast<uint32_t>(frameBufferSizeX), static_cast<uint32_t>(frameBufferSizeY), 1};
    textureDesc.dimension = wgpu::TextureDimension::_2D;
    textureDesc.mipLevelCount = 1;
    textureDesc.sampleCount = 1;
    textureDesc.format = wgpu::TextureFormat::Depth24Plus;
    textureDesc.usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::RenderAttachment |
                        wgpu::TextureUsage::CopySrc | wgpu::TextureUsage::CopyDst;
    textureDesc.viewFormats = nullptr;
    textureDesc.viewFormatCount = 0;

    Graphic::Resource::Texture depthTexture(context, textureDesc);

    core.GetResource<Graphic::Resource::TextureContainer>().Add(
        entt::hashed_string{textureName.data(), textureName.size()}, std::move(depthTexture));
}

void Graphic::System::CreateDefaultRenderPipeline(Engine::Core &core)
{
    auto &renderPassContainer = core.GetResource<Graphic::Resource::RenderGraphContainer>();

    const std::string END_DEPTH_RENDER_TEXTURE_NAME = "END_DEPTH_RENDER_TEXTURE";
    CreateEndDepthRenderTexture(core, END_DEPTH_RENDER_TEXTURE_NAME);

    Graphic::Resource::RenderGraph renderGraph{};
    {
        Graphic::Utils::DefaultRenderPass renderPass{};
        Graphic::Resource::Shader defaultShader =
            Graphic::Utils::DefaultRenderPass::CreateShader(core.GetResource<Graphic::Resource::Context>());
        core.GetResource<Graphic::Resource::ShaderContainer>().Add(Graphic::Utils::DEFAULT_RENDER_PASS_SHADER_ID,
                                                                   std::move(defaultShader));
        renderPass.BindShader(std::string(Graphic::Utils::DEFAULT_RENDER_PASS_SHADER_NAME));
        Graphic::Resource::ColorOutput colorOutput;
        colorOutput.textureId = System::END_RENDER_TEXTURE_ID;
        renderPass.AddOutput(0, std::move(colorOutput));
        Graphic::Resource::DepthOutput depthOutput;
        depthOutput.textureId =
            entt::hashed_string{END_DEPTH_RENDER_TEXTURE_NAME.data(), END_DEPTH_RENDER_TEXTURE_NAME.size()};
        depthOutput.getClearColorCallback = [](Engine::Core &, float &clearDepth) {
            clearDepth = 1.0f;
            return true;
        };
        depthOutput.storeOp = wgpu::StoreOp::Store;
        renderPass.AddOutput(std::move(depthOutput));
        renderGraph.Add(Graphic::Utils::DEFAULT_RENDER_PASS_NAME, std::move(renderPass));
    }

    {
        Graphic::Utils::DefaultRenderPass renderPass{};
        Graphic::Resource::Shader defaultShader =
            Graphic::Utils::DefaultRenderPass::CreateShader(core.GetResource<Graphic::Resource::Context>());
        core.GetResource<Graphic::Resource::ShaderContainer>().Add(Graphic::Utils::DEFAULT_RENDER_PASS_SHADER_ID,
                                                                   std::move(defaultShader));
        renderPass.BindShader(std::string(Graphic::Utils::DEFAULT_RENDER_PASS_SHADER_NAME));
        Graphic::Resource::ColorOutput colorOutput;
        colorOutput.textureId = System::END_RENDER_TEXTURE_ID;
        renderPass.AddOutput(0, std::move(colorOutput));
        renderGraph.Add(Graphic::Utils::DEFAULT_RENDER_PASS_NAME, std::move(renderPass));
    }

    renderPassContainer.Add(Graphic::Utils::DEFAULT_RENDER_GRAPH_ID, std::move(renderGraph));
}
