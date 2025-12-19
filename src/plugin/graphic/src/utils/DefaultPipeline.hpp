#pragma once

#include "resource/SingleExecutionRenderPass.hpp"
#include "resource/Shader.hpp"
#include "resource/ShaderDescriptor.hpp"
#include <entt/core/hashed_string.hpp>

namespace Graphic::Utils {
static inline constexpr std::string_view DEFAULT_RENDER_GRAPH_NAME = "END_RENDER_TEXTURE";
static inline const entt::hashed_string DEFAULT_RENDER_GRAPH_ID{DEFAULT_RENDER_GRAPH_NAME.data(),
                                                                    DEFAULT_RENDER_GRAPH_NAME.size()};
static inline constexpr std::string_view DEFAULT_RENDER_PASS_NAME = "DEFAULT_RENDER_PASS";
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
    const color = vec4f(27.0 / 255.0, 7.0 / 255.0, 7.0 / 255.0, 1.0);
    return color;
}

)";

class DefaultRenderPass : public Graphic::Resource::ASingleExecutionRenderPass<DefaultRenderPass> {
  public:
    explicit DefaultRenderPass() : ASingleExecutionRenderPass(DEFAULT_RENDER_PASS_NAME) {}

    void UniqueRenderCallback(wgpu::RenderPassEncoder &renderPass, Engine::Core &core) override
    {
        renderPass.draw(6, 1, 0, 0);
    }

    static Graphic::Resource::Shader CreateShader(Graphic::Resource::Context &graphicContext)
    {
        Graphic::Resource::ShaderDescriptor shaderDescriptor;

        auto output =
            Graphic::Utils::ColorTargetState("END_RENDER_TEXTURE").setFormat(wgpu::TextureFormat::BGRA8UnormSrgb);

        shaderDescriptor.setShader(DEFAULT_RENDER_PASS_SHADER_CONTENT)
            .setName(DEFAULT_RENDER_PASS_SHADER_NAME)
            .addOutputColorFormat(output);
        return Graphic::Resource::Shader::Create(shaderDescriptor, graphicContext);
    }
};
}
