#pragma once

#include "core/Core.hpp"
#include "resource/Context.hpp"
#include "resource/Shader.hpp"
#include "resource/ShaderContainer.hpp"
#include "resource/ShaderDescriptor.hpp"
#include "resource/SingleExecutionRenderPass.hpp"
#include "system/preparation/CreateEndRenderTexture.hpp"
#include "utils/shader/ColorTargetState.hpp"
#include <string>

namespace Graphic::Resource {

static inline constexpr std::string_view DEFAULT_RENDER_PASS_SHADER_NAME = "DEFAULT_RENDER_PASS_SHADER";
static inline const entt::hashed_string DEFAULT_RENDER_PASS_SHADER_ID{DEFAULT_RENDER_PASS_SHADER_NAME.data(),
                                                                      DEFAULT_RENDER_PASS_SHADER_NAME.size()};
static inline constexpr std::string_view DEFAULT_RENDER_PASS_SHADER_CONTENT = R"(
@vertex
fn vs_main(
  @builtin(vertex_index) VertexIndex : u32
) -> @builtin(position) vec4f {
  const pos = array(
    vec2(-1.0, -1.0), vec2(1.0, -1.0), vec2(-1.0, 1.0),
    vec2(-1.0, 1.0), vec2(1.0, -1.0), vec2(1.0, 1.0),
  );

  return vec4f(pos[VertexIndex], 0.9, 1.0);
}

@fragment
fn fs_main() -> @location(0) vec4f {
    const color = vec4f(27.0 / 255.0, 7.0 / 255.0, 7.0 / 255.0, 255.0);
    return color;
}

)";

class DefaultRenderPass : public ASingleExecutionRenderPass<DefaultRenderPass> {
  public:
    explicit DefaultRenderPass() : ASingleExecutionRenderPass("DefaultRenderPass") {}

    void UniqueRenderCallback(wgpu::RenderPassEncoder &renderPass, Engine::Core &core) override
    {
        renderPass.draw(6, 1, 0, 0);
    }

    static Shader CreateShader(Context &graphicContext)
    {
        ShaderDescriptor shaderDescriptor;

        auto output =
            Graphic::Utils::ColorTargetState("END_RENDER_TEXTURE").setFormat(wgpu::TextureFormat::BGRA8UnormSrgb);

        shaderDescriptor.setShader(DEFAULT_RENDER_PASS_SHADER_CONTENT)
            .setName(DEFAULT_RENDER_PASS_SHADER_NAME)
            .addOutputColorFormat(output);
        return Shader::Create(shaderDescriptor, graphicContext);
    }
};

struct RenderPassManager {
    static inline constexpr std::string_view DEFAULT_RENDER_PASS_NAME = "DEFAULT_RENDER_PASS";

    RenderPassManager() = default;
    ~RenderPassManager() = default;

    void Execute(Engine::Core &core)
    {
        if (_defaultRenderPass == nullptr)
            _defaultRenderPass = std::make_shared<DefaultRenderPass>(_CreateDefaultRenderPass(core));
        _defaultRenderPass->Execute(core);
    }

  private:
    DefaultRenderPass _CreateDefaultRenderPass(Engine::Core &core)
    {
        DefaultRenderPass renderPass{};
        Shader defaultShader = DefaultRenderPass::CreateShader(core.GetResource<Context>());
        core.GetResource<ShaderContainer>().Add(DEFAULT_RENDER_PASS_SHADER_ID, std::move(defaultShader));
        Graphic::Resource::ColorOutput colorOutput;
        colorOutput.textureViewId = System::END_RENDER_TEXTURE_ID;
        renderPass.BindShader(std::string(DEFAULT_RENDER_PASS_SHADER_NAME)).AddOutput(0, colorOutput);
        return renderPass;
    }

    std::shared_ptr<DefaultRenderPass> _defaultRenderPass = nullptr;
};
} // namespace Graphic::Resource
