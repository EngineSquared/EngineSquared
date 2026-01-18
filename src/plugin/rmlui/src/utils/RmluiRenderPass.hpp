#pragma once

#include "core/Core.hpp"
#include "entt/core/fwd.hpp"

#include "resource/Context.hpp"
#include "resource/Shader.hpp"
#include "resource/SingleExecutionRenderPass.hpp"

#include "utils/webgpu.hpp"
#include "utils/RenderInterface.hpp"

#include <entt/core/hashed_string.hpp>
#include <string_view>

namespace Rmlui::Utils {
static inline constexpr std::string_view RMLUI_RENDER_PASS_NAME = "RMLUI_RENDER_PASS";
static inline constexpr std::string_view RMLUI_RENDER_PASS_SHADER_NAME = "RMLUI_RENDER_PASS_SHADER";
static inline const entt::hashed_string RMLUI_RENDER_PASS_SHADER_ID{RMLUI_RENDER_PASS_SHADER_NAME.data(),
                                                                    RMLUI_RENDER_PASS_SHADER_NAME.size()};

static inline constexpr std::string_view RMLUI_RENDER_PASS_SHADER_CONTENT = R"(
struct ScreenData {
    size : vec2f,
    _pad : vec2f,
};

@group(0) @binding(0) var uiTexture : texture_2d<f32>;
@group(0) @binding(1) var uiSampler : sampler;
@group(1) @binding(0) var<uniform> screen : ScreenData;

struct VertexInput {
    @location(0) position : vec2f,
    @location(1) color : vec4<u32>,
    @location(2) uv : vec2f,
};

struct VertexOutput {
    @builtin(position) Position : vec4f,
    @location(0) color : vec4f,
    @location(1) uv : vec2f,
};

@vertex
fn vs_main(input : VertexInput) -> VertexOutput {
    var output : VertexOutput;
    let ndc = vec2f(
        (input.position.x / screen.size.x) * 2.0 - 1.0,
        1.0 - (input.position.y / screen.size.y) * 2.0
    );
    output.Position = vec4f(ndc, 0.0, 1.0);
    output.color = vec4f(input.color) / 255.0;
    output.uv = input.uv;
    return output;
}

@fragment
fn fs_main(input : VertexOutput) -> @location(0) vec4f {
    let texColor = textureSample(uiTexture, uiSampler, input.uv);
    return input.color * texColor;
}
)";

class RmluiRenderPass : public Graphic::Resource::ASingleExecutionRenderPass<RmluiRenderPass> {
  public:
    explicit RmluiRenderPass() : ASingleExecutionRenderPass(RMLUI_RENDER_PASS_NAME) {}
    virtual ~RmluiRenderPass() = default;

    void UniqueRenderCallback(wgpu::RenderPassEncoder &renderPass, Engine::Core &core) override
    {
        (void) core; // TODO: Might not be needed in the context
        auto *renderer = RenderInterface::GetActive();
        if (renderer == nullptr)
        {
            return;
        }
        renderer->FlushDrawCommands(renderPass);
    }

    static Graphic::Resource::Shader CreateShader(Graphic::Resource::Context &graphicContext);
};
} // namespace Rmlui::Utils
