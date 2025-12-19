#include "utils/DefaultPipeline.hpp"
#include "system/initialization/CreateDefaultRenderPipeline.hpp"
#include "system/preparation/CreateEndRenderTexture.hpp"
#include "resource/RenderGraphContainer.hpp"
#include "resource/ShaderContainer.hpp"
#include "resource/RenderGraph.hpp"

void Graphic::System::CreateDefaultRenderPipeline(Engine::Core &core)
{
    auto &renderPassContainer = core.GetResource<Graphic::Resource::RenderGraphContainer>();

    Graphic::Resource::RenderGraph renderGraph{};

    {
      Graphic::Utils::DefaultRenderPass renderPass{};
      Graphic::Resource::Shader defaultShader =
          Graphic::Utils::DefaultRenderPass::CreateShader(core.GetResource<Graphic::Resource::Context>());
      core.GetResource<Graphic::Resource::ShaderContainer>().Add(Graphic::Utils::DEFAULT_RENDER_PASS_SHADER_ID, std::move(defaultShader));
      renderPass.BindShader(std::string(Graphic::Utils::DEFAULT_RENDER_PASS_SHADER_NAME));
      Graphic::Resource::ColorOutput colorOutput;
      colorOutput.textureId = System::END_RENDER_TEXTURE_ID;
      renderPass.AddOutput(0, std::move(colorOutput));
      renderGraph.Add(Graphic::Utils::DEFAULT_RENDER_PASS_NAME, std::move(renderPass));
    }

    renderPassContainer.Add(Graphic::Utils::DEFAULT_RENDER_GRAPH_ID, std::move(renderGraph));
}
