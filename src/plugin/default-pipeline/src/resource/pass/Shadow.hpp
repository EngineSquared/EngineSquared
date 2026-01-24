#pragma once

#include "Logger.hpp"
#include "component/GPUCamera.hpp"
#include "component/GPUMaterial.hpp"
#include "component/GPUMesh.hpp"
#include "component/GPUTransform.hpp"
#include "core/Core.hpp"
#include "entity/Entity.hpp"
#include "resource/ASingleExecutionRenderPass.hpp"
#include "resource/buffer/CameraGPUBuffer.hpp"
#include "resource/buffer/PointLightsBuffer.hpp"
#include "utils/DefaultMaterial.hpp"
#include "utils/PointLights.hpp"
#include "utils/shader/BufferBindGroupLayoutEntry.hpp"
#include "utils/shader/SamplerBindGroupLayoutEntry.hpp"
#include "utils/shader/TextureBindGroupLayoutEntry.hpp"
#include <entt/core/hashed_string.hpp>
#include <string_view>

namespace DefaultPipeline::Resource {
static inline constexpr std::string_view DEFERRED_PASS_OUTPUT = "DEFERRED_PASS_OUTPUT";
static inline const entt::hashed_string DEFERRED_PASS_OUTPUT_ID{DEFERRED_PASS_OUTPUT.data(),
                                                                DEFERRED_PASS_OUTPUT.size()};
static inline constexpr std::string_view DEFERRED_PASS_NAME = "DEFERRED_PASS";
static inline const entt::hashed_string DEFERRED_PASS_ID{DEFERRED_PASS_NAME.data(), DEFERRED_PASS_NAME.size()};
static inline constexpr std::string_view DEFERRED_SHADER_NAME = "DEFERRED_SHADER";
static inline const entt::hashed_string DEFERRED_SHADER_ID =
    entt::hashed_string{DEFERRED_SHADER_NAME.data(), DEFERRED_SHADER_NAME.size()};

static inline constexpr std::string_view DEFERRED_BINDGROUP_TEXTURES_NAME = "DEFERRED_BINDGROUP_TEXTURES";
static inline const entt::hashed_string DEFERRED_BINDGROUP_TEXTURES_ID =
    entt::hashed_string{DEFERRED_BINDGROUP_TEXTURES_NAME.data(), DEFERRED_BINDGROUP_TEXTURES_NAME.size()};

static inline constexpr std::string_view DEFERRED_SHADE_CONTENT = R"(
const MAX_POINT_LIGHTS: u32 = 64u;

struct Input {
    @builtin(vertex_index) VertexIndex : u32
};

struct VertexToFragment {
    @builtin(position) coord : vec4f
}

struct Output {
    @location(0) color : vec4f,
}

struct Light {
  lightViewProjMatrix: mat4x4f,
  color: vec4f,
  direction: vec3f,
};

struct Object {
  model : mat4x4<f32>,
  normal : mat4x4<f32>,
}



@vertex
fn vs_main(
    input : DeferredInput
) -> VertexToFragment {
    var coord : vec4f;
    const pos = array(
        vec2(-1.0, -1.0), vec2(-1.0, 1.0), vec2(1.0, -1.0),
        vec2(-1.0, 1.0), vec2(1.0, 1.0), vec2(1.0, -1.0)
    );

    coord = vec4f(pos[input.VertexIndex], 0.9, 1.0);
    return VertexToFragment(coord);
}

fn world_from_screen_coord(coord : vec2f, depth_sample: f32) -> vec3f {
  let posClip = vec4(coord.x * 2.0 - 1.0, (1.0 - coord.y) * 2.0 - 1.0, depth_sample, 1.0);
  let posWorldW = camera.invViewProjectionMatrix * posClip;
  let posWorld = posWorldW.xyz / posWorldW.www;
  return posWorld;
}

// Physically plausible point-light attenuation with finite radius
// Formula inside the radius: A * (1 - s^2)^2 / (1 + F * s), where s = d / R
// For s >= 1 (distance >= R) the attenuation is explicitly clamped to 0.0.
// This yields a compact-support profile that is C1-smooth at distance R (value and derivative are zero there).
// See https://lisyarus.github.io/blog/posts/point-light-attenuation.html for more details on this model.
fn attenuate(distance: f32, radius: f32, max_intensity: f32, falloff: f32) -> f32 {
    let s = distance / radius;

    if (s >= 1.0) {
        return 0.0;
    }

    let s2 = s * s;
    let one_minus_s2 = 1.0 - s2;

    return max_intensity * one_minus_s2 * one_minus_s2 / (1.0 + falloff * s);
}

fn calculatePointLight(light: GPUPointLight, worldPos: vec3f, normal: vec3f) -> vec3f {
    let lightDir = normalize(light.position - worldPos);
    let distance = length(light.position - worldPos);
    let attenuation = attenuate(distance, light.radius, light.intensity, light.falloff);
    let diff = max(dot(normal, lightDir), 0.0);

    return light.color * diff * attenuation;
}

@fragment
fn fs_main(
  vertexToFragment : VertexToFragment,
) -> DeferredOutput {
    var output : DeferredOutput;
    output.color = vec4(0.0, 0.0, 0.0, 1.0);
    var coords = vec2i(floor(vertexToFragment.coord.xy));

    let depth = textureLoad(gBufferDepth, coords, 0).x;

    if (depth >= 1.0) {
        return output;
    }

    let bufferSize = textureDimensions(gBufferDepth);
    let coordUV = floor(vertexToFragment.coord.xy) / vec2f(bufferSize);
    let position = world_from_screen_coord(coordUV, depth);

    let normal = textureLoad(gBufferNormal, coords, 0).xyz;
    let albedo = textureLoad(gBufferAlbedo, coords, 0).rgb;

    let N = normalize(normal);
    let V = normalize(camera.position - position);

    var lighting = ambientLight.color;

    for (var i = 0u; i < pointLights.count; i++) {
        lighting += calculatePointLight(pointLights.lights[i], position, N);
    }

    var color : vec4f = vec4f(albedo * lighting, 1.0);
    output.color = color;
    return output;
}
)";

class Deferred : public Graphic::Resource::ASingleExecutionRenderPass<Deferred> {
  public:
    explicit Deferred(std::string_view name = DEFERRED_PASS_NAME) : ASingleExecutionRenderPass<Deferred>(name) {}

    void UniqueRenderCallback(wgpu::RenderPassEncoder &renderPass, Engine::Core &core) override
    {
        const auto &bindGroupManager = core.GetResource<Graphic::Resource::BindGroupManager>();
        const auto &bufferContainer = core.GetResource<Graphic::Resource::GPUBufferContainer>();

        auto cameraView = core.GetRegistry().view<Component::GPUCamera>();
        if (cameraView.empty())
        {
            Log::Error("Deferred::UniqueRenderCallback: No camera with GPUCamera component found.");
            return;
        }
        Engine::Entity camera{core, cameraView.front()};
        const auto &cameraGPUComponent = camera.GetComponents<Component::GPUCamera>();
        const auto &cameraBindGroup = bindGroupManager.Get(cameraGPUComponent.bindGroup);
        renderPass.setBindGroup(cameraBindGroup.GetLayoutIndex(), cameraBindGroup.GetBindGroup(), 0, nullptr);

        const auto &lightsBindGroup = bindGroupManager.Get(Utils::LIGHTS_BIND_GROUP_ID);
        renderPass.setBindGroup(lightsBindGroup.GetLayoutIndex(), lightsBindGroup.GetBindGroup(), 0, nullptr);

        const auto &texturesBindgroup = bindGroupManager.Get(DEFERRED_BINDGROUP_TEXTURES_ID);
        renderPass.setBindGroup(texturesBindgroup.GetLayoutIndex(), texturesBindgroup.GetBindGroup(), 0, nullptr);

        renderPass.draw(6, 1, 0, 0);
    }

    static Graphic::Resource::Shader CreateShader(Graphic::Resource::Context &graphicContext)
    {
        Graphic::Resource::ShaderDescriptor shaderDescriptor;

        auto gBufferTexturesLayout = Graphic::Utils::BindGroupLayout("gBufferTextures")
                                         .addEntry(Graphic::Utils::TextureBindGroupLayoutEntry("normal")
                                                       .setSampleType(wgpu::TextureSampleType::UnfilterableFloat)
                                                       .setViewDimension(wgpu::TextureViewDimension::_2D)
                                                       .setVisibility(wgpu::ShaderStage::Fragment)
                                                       .setBinding(0))
                                         .addEntry(Graphic::Utils::TextureBindGroupLayoutEntry("albedo")
                                                       .setSampleType(wgpu::TextureSampleType::UnfilterableFloat)
                                                       .setViewDimension(wgpu::TextureViewDimension::_2D)
                                                       .setVisibility(wgpu::ShaderStage::Fragment)
                                                       .setBinding(1))
                                         .addEntry(Graphic::Utils::TextureBindGroupLayoutEntry("depth")
                                                       .setSampleType(wgpu::TextureSampleType::UnfilterableFloat)
                                                       .setViewDimension(wgpu::TextureViewDimension::_2D)
                                                       .setVisibility(wgpu::ShaderStage::Fragment)
                                                       .setBinding(2));
        auto cameraLayout = Graphic::Utils::BindGroupLayout("camera").addEntry(
            Graphic::Utils::BufferBindGroupLayoutEntry("camera")
                .setType(wgpu::BufferBindingType::Uniform)
                .setMinBindingSize(Resource::CameraGPUBuffer::CameraTransfer::GPUSize())
                .setVisibility(wgpu::ShaderStage::Fragment | wgpu::ShaderStage::Vertex)
                .setBinding(0));
        auto lightsLayout = Graphic::Utils::BindGroupLayout("LightsLayout")
                                .addEntry(Graphic::Utils::BufferBindGroupLayoutEntry("ambientLight")
                                              .setType(wgpu::BufferBindingType::Uniform)
                                              .setMinBindingSize(sizeof(glm::vec3) + sizeof(float) /*padding*/)
                                              .setVisibility(wgpu::ShaderStage::Fragment)
                                              .setBinding(0))
                                .addEntry(Graphic::Utils::BufferBindGroupLayoutEntry("pointLights")
                                              .setType(wgpu::BufferBindingType::Uniform)
                                              .setMinBindingSize(Resource::PointLightsBuffer::GPUSize())
                                              .setVisibility(wgpu::ShaderStage::Fragment)
                                              .setBinding(1));

        auto colorOutput =
            Graphic::Utils::ColorTargetState("DEFERRED_OUTPUT").setFormat(wgpu::TextureFormat::BGRA8UnormSrgb);

        shaderDescriptor.setShader(DEFERRED_SHADE_CONTENT)
            .setName(DEFERRED_SHADER_NAME)
            .setVertexEntryPoint("vs_main")
            .setFragmentEntryPoint("fs_main")
            .addBindGroupLayout(cameraLayout)
            .addBindGroupLayout(gBufferTexturesLayout)
            .addBindGroupLayout(lightsLayout)
            .addOutputColorFormat(colorOutput);
        const auto validations = shaderDescriptor.validate();
        if (!validations.empty())
        {
            for (const auto &validation : validations)
            {
                if (validation.severity == Graphic::Utils::ValidationError::Severity::Error)
                {
                    Log::Error(fmt::format("Shader Descriptor Validation Error: {} at {}", validation.message,
                                           validation.location));
                }
                else if (validation.severity == Graphic::Utils::ValidationError::Severity::Warning)
                {
                    Log::Warn(fmt::format("Shader Descriptor Validation Warning: {} at {}", validation.message,
                                          validation.location));
                }
            }
        }
        return Graphic::Resource::Shader::Create(shaderDescriptor, graphicContext);
    }
};

} // namespace DefaultPipeline::Resource
