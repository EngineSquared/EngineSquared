#include "system/initialization/CreateDefaultRenderPipeline.hpp"
#include "resource/ARenderPass.hpp"
#include "resource/ASingleExecutionRenderPass.hpp"
#include "resource/RenderGraph.hpp"
#include "resource/RenderGraphContainer.hpp"
#include "resource/ShaderContainer.hpp"
#include "resource/Window.hpp"
#include "system/preparation/PrepareEndRenderTexture.hpp"
#include "utils/DefaultRenderPass.hpp"
#include "utils/EndRenderTexture.hpp"

void DefaultPipeline::System::CreateDefaultRenderPipeline(Engine::Core &core)
{
    auto &renderPassContainer = core.GetResource<Graphic::Resource::RenderGraphContainer>();

    Graphic::Resource::RenderGraph renderGraph{};
    {
        Utils::DefaultRenderPass renderPass{};
        Graphic::Resource::Shader defaultShader =
            Utils::DefaultRenderPass::CreateShader(core.GetResource<Graphic::Resource::Context>());
        core.GetResource<Graphic::Resource::ShaderContainer>().Add(Utils::DEFAULT_RENDER_PASS_SHADER_ID,
                                                                   std::move(defaultShader));
        renderPass.BindShader(std::string(Utils::DEFAULT_RENDER_PASS_SHADER_NAME));
        Graphic::Resource::ColorOutput colorOutput;
        colorOutput.textureId = Graphic::Utils::END_RENDER_TEXTURE_ID;
        renderPass.AddOutput(0, std::move(colorOutput));
        Graphic::Resource::DepthOutput depthOutput;
        depthOutput.textureId = Graphic::System::END_DEPTH_RENDER_TEXTURE_ID;
        depthOutput.getClearDepthCallback = [](Engine::Core &, float &clearDepth) {
            clearDepth = 1.0f;
            return true;
        };
        depthOutput.storeOp = wgpu::StoreOp::Store;
        renderPass.AddOutput(std::move(depthOutput));
        renderGraph.Add(Utils::DEFAULT_RENDER_PASS_NAME, std::move(renderPass));
    }

    renderPassContainer.SetDefault(std::move(renderGraph));
}
