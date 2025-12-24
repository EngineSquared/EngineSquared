#include "system/initialization/CreateDefaultRenderPipeline.hpp"
#include "resource/ARenderPass.hpp"
#include "resource/RenderGraph.hpp"
#include "resource/RenderGraphContainer.hpp"
#include "resource/ShaderContainer.hpp"
#include "resource/SingleExecutionRenderPass.hpp"
#include "resource/Window.hpp"
#include "system/preparation/PrepareEndRenderTexture.hpp"
#include "utils/DefaultPipeline.hpp"

void Graphic::System::CreateDefaultRenderPipeline(Engine::Core &core)
{
    auto &renderPassContainer = core.GetResource<Graphic::Resource::RenderGraphContainer>();

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
        depthOutput.textureId = END_DEPTH_RENDER_TEXTURE_ID;
        depthOutput.getClearColorCallback = [](Engine::Core &, float &clearDepth) {
            clearDepth = 1.0f;
            return true;
        };
        depthOutput.storeOp = wgpu::StoreOp::Store;
        renderPass.AddOutput(std::move(depthOutput));
        renderGraph.Add(Graphic::Utils::DEFAULT_RENDER_PASS_NAME, std::move(renderPass));
    }

    renderPassContainer.Add(Graphic::Utils::DEFAULT_RENDER_GRAPH_ID, std::move(renderGraph));
}
