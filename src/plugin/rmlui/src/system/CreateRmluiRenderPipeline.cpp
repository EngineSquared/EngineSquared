#include <string_view>
#include <utility>

#include "Logger.hpp"
#include "core/Core.hpp"

#include "system/CreateRmluiRenderPipeline.hpp"

#include "resource/ARenderPass.hpp"
#include "resource/Context.hpp"
#include "resource/RenderGraph.hpp"
#include "resource/RenderGraphContainer.hpp"
#include "resource/Shader.hpp"
#include "resource/ShaderContainer.hpp"

#include "utils/EndRenderTexture.hpp"
#include "utils/RmluiRenderPass.hpp"
#include "utils/webgpu.hpp"

void Rmlui::System::CreateRmluiRenderPipeline(Engine::Core &core)
{
    auto &renderGraphContainer = core.GetResource<Graphic::Resource::RenderGraphContainer>();
    if (!renderGraphContainer.HasDefault())
    {
        Log::Warn("Rmlui render pass skipped: no default render graph available yet.");
        return;
    }

    auto &renderGraph = renderGraphContainer.GetDefault();
    if (renderGraph.Contains(Rmlui::Utils::RMLUI_RENDER_PASS_NAME))
    {
        return;
    }

    Utils::RmluiRenderPass renderPass{};
    Graphic::Resource::Shader shader =
        Utils::RmluiRenderPass::CreateShader(core.GetResource<Graphic::Resource::Context>());
    core.GetResource<Graphic::Resource::ShaderContainer>().Add(Utils::RMLUI_RENDER_PASS_SHADER_ID, std::move(shader));
    renderPass.BindShader(std::string_view(Utils::RMLUI_RENDER_PASS_SHADER_NAME));

    Graphic::Resource::ColorOutput colorOutput;
    colorOutput.textureId = Graphic::Utils::END_RENDER_TEXTURE_ID;
    colorOutput.storeOp = wgpu::StoreOp::Store;
    renderPass.AddOutput(0, std::move(colorOutput));

    renderGraph.Add(Utils::RMLUI_RENDER_PASS_NAME, std::move(renderPass));
    if (renderGraph.Contains("DEFERRED_PASS"))
    {
        renderGraph.SetDependency("DEFERRED_PASS", Utils::RMLUI_RENDER_PASS_NAME);
    }
}
