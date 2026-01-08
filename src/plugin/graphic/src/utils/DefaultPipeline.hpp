#pragma once

#include "component/GPUCamera.hpp"
#include "component/GPUMaterial.hpp"
#include "component/GPUMesh.hpp"
#include "component/GPUTransform.hpp"
#include "component/Transform.hpp"
#include "entity/Entity.hpp"
#include "resource/AmbientLight.hpp"
#include "resource/PointLights.hpp"
#include "resource/Shader.hpp"
#include "resource/ShaderDescriptor.hpp"
#include "resource/SingleExecutionRenderPass.hpp"
#include "resource/buffer/PointLightsBuffer.hpp"
#include "utils/AmbientLight.hpp"
#include "utils/DefaultMaterial.hpp"
#include "utils/PointLight.hpp"
#include "utils/shader/BufferBindGroupLayoutEntry.hpp"
#include "utils/shader/SamplerBindGroupLayoutEntry.hpp"
#include "utils/shader/TextureBindGroupLayoutEntry.hpp"
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
const MAX_POINT_LIGHTS: u32 = 64u;

struct Camera {
    viewProjectionMatrix : mat4x4<f32>,
};

struct Model {
    modelMatrix : mat4x4<f32>,
};

struct Material {
    emission: vec3f,
    padding: f32,
};

struct AmbientLight {
    color : vec3f,
    padding : f32,
};

struct GPUPointLight {
    position: vec3f,
    intensity: f32,
    color: vec3f,
    radius: f32,
    falloff: f32,
    _padding1: f32,
    _padding2: f32,
    _padding3: f32,
};

struct PointLightsData {
    lights: array<GPUPointLight, MAX_POINT_LIGHTS>,
    count: u32,
    _padding1: f32,
    _padding2: f32,
    _padding3: f32,
};

@group(0) @binding(0) var<uniform> camera : Camera;
@group(1) @binding(0) var<uniform> model : Model;
@group(2) @binding(0) var<uniform> material : Material;
@group(2) @binding(1) var materialTexture : texture_2d<f32>;
@group(2) @binding(2) var materialSampler : sampler;
@group(3) @binding(0) var<uniform> ambientLight : AmbientLight;
@group(3) @binding(1) var<uniform> pointLights : PointLightsData;

struct VertexInput {
    @location(0) position : vec3f,
    @location(1) normal : vec3f,
    @location(2) uv : vec2f,
};

struct VertexOutput {
    @builtin(position) Position : vec4f,
    @location(0) fragUV : vec2f,
    @location(1) worldPos : vec3f,
    @location(2) worldNormal : vec3f,
};

@vertex
fn vs_main(
    input : VertexInput
) -> VertexOutput {
    var output : VertexOutput;
    let worldPos = model.modelMatrix * vec4f(input.position, 1.0);
    output.Position = camera.viewProjectionMatrix * worldPos;
    output.fragUV = input.uv;
    output.worldPos = worldPos.xyz;
    output.worldNormal = (model.modelMatrix * vec4f(input.normal, 0.0)).xyz;
    return output;
}

// Attempt at physically correct attenuation with finite radius
// Formula: A * (1 - s^2)^2 / (1 + F * s) where s = d/R
// This gives exactly zero at distance R with zero derivative (smooth cutoff)
// See https://lisyarus.github.io/blog/posts/point-light-attenuation.html for more information
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
    input : VertexOutput
) -> @location(0) vec4f {
    var uv = vec2f(1.0 - input.fragUV.x, 1.0 - input.fragUV.y);
    var texColor : vec4f = textureSample(materialTexture, materialSampler, uv);
    let normal = normalize(input.worldNormal);

    var lighting = ambientLight.color;

    for (var i = 0u; i < pointLights.count; i++) {
        lighting += calculatePointLight(pointLights.lights[i], input.worldPos, normal);
    }

    var color : vec4f = vec4f(material.emission * texColor.xyz * lighting, texColor.a);
    return color;
}

)";

class DefaultRenderPass : public Graphic::Resource::ASingleExecutionRenderPass<DefaultRenderPass> {
  public:
    explicit DefaultRenderPass(void) : ASingleExecutionRenderPass(DEFAULT_RENDER_PASS_NAME) {}

    void UniqueRenderCallback(wgpu::RenderPassEncoder &renderPass, Engine::Core &core) override
    {
        Engine::Entity camera(core.GetRegistry().view<Graphic::Component::GPUCamera>().front());
        const auto &cameraGPUComponent = camera.GetComponents<Graphic::Component::GPUCamera>(core);
        const auto &bindGroupManager = core.GetResource<Graphic::Resource::BindGroupManager>();

        const auto &cameraBindGroup = bindGroupManager.Get(cameraGPUComponent.bindGroup);
        renderPass.setBindGroup(0, cameraBindGroup.GetBindGroup(), 0, nullptr);

        const auto &lightsBindGroup = bindGroupManager.Get(Graphic::Utils::LIGHTS_BIND_GROUP_ID);
        renderPass.setBindGroup(3, lightsBindGroup.GetBindGroup(), 0, nullptr);

        const auto &bufferContainer = core.GetResource<Graphic::Resource::GPUBufferContainer>();
        const auto &bindgroupContainer = core.GetResource<Graphic::Resource::BindGroupManager>();

        auto view = core.GetRegistry().view<Graphic::Component::GPUTransform, Graphic::Component::GPUMesh>();

        for (auto &&[e, transform, gpuMesh] : view.each())
        {
            Engine::Entity entity(e);

            const auto &transformBindgroup = bindgroupContainer.Get(transform.bindGroup);
            renderPass.setBindGroup(transformBindgroup.GetLayoutIndex(), transformBindgroup.GetBindGroup(), 0, nullptr);

            // TODO: create a system that will add the component if not present before rendering to avoid checking every
            // frame if the component exists
            entt::hashed_string gpuMaterialId{};
            if (entity.HasComponents<Graphic::Component::GPUMaterial>(core))
            {
                const auto &materialComponent = entity.GetComponents<Graphic::Component::GPUMaterial>(core);
                gpuMaterialId = materialComponent.bindGroup;
            }
            else
            {
                gpuMaterialId = Utils::DEFAULT_MATERIAL_BIND_GROUP_ID;
            }
            const auto &materialBindgroup = bindgroupContainer.Get(gpuMaterialId);
            renderPass.setBindGroup(materialBindgroup.GetLayoutIndex(), materialBindgroup.GetBindGroup(), 0, nullptr);

            const auto &pointBuffer = bufferContainer.Get(gpuMesh.pointBufferId);
            const auto &pointBufferSize = pointBuffer->GetBuffer().getSize();
            renderPass.setVertexBuffer(0, pointBuffer->GetBuffer(), 0, pointBufferSize);
            const auto &indexBuffer = bufferContainer.Get(gpuMesh.indexBufferId);
            const auto &indexBufferSize = indexBuffer->GetBuffer().getSize();
            renderPass.setIndexBuffer(indexBuffer->GetBuffer(), wgpu::IndexFormat::Uint32, 0, indexBufferSize);

            renderPass.drawIndexed(indexBufferSize / sizeof(uint32_t), 1, 0, 0, 0);
        }
    }

    static Graphic::Resource::Shader CreateShader(Graphic::Resource::Context &graphicContext)
    {
        Graphic::Resource::ShaderDescriptor shaderDescriptor;

        auto cameraLayout = Graphic::Utils::BindGroupLayout("CameraModelLayout")
                                .addEntry(Graphic::Utils::BufferBindGroupLayoutEntry("camera")
                                              .setType(wgpu::BufferBindingType::Uniform)
                                              .setMinBindingSize(sizeof(glm::mat4))
                                              .setVisibility(wgpu::ShaderStage::Vertex)
                                              .setBinding(0));
        auto modelLayout = Graphic::Utils::BindGroupLayout("CameraModelLayout")
                               .addEntry(Graphic::Utils::BufferBindGroupLayoutEntry("model")
                                             .setType(wgpu::BufferBindingType::Uniform)
                                             .setMinBindingSize(sizeof(glm::mat4))
                                             .setVisibility(wgpu::ShaderStage::Vertex)
                                             .setBinding(0));
        auto materialLayout = Graphic::Utils::BindGroupLayout("MaterialLayout")
                                  .addEntry(Graphic::Utils::BufferBindGroupLayoutEntry("material")
                                                .setType(wgpu::BufferBindingType::Uniform)
                                                .setMinBindingSize(sizeof(glm::vec3) + sizeof(float) /*padding*/)
                                                .setVisibility(wgpu::ShaderStage::Fragment)
                                                .setBinding(0))
                                  .addEntry(Graphic::Utils::TextureBindGroupLayoutEntry("materialTexture")
                                                .setSampleType(wgpu::TextureSampleType::Float)
                                                .setViewDimension(wgpu::TextureViewDimension::_2D)
                                                .setVisibility(wgpu::ShaderStage::Fragment)
                                                .setBinding(1))
                                  .addEntry(Graphic::Utils::SamplerBindGroupLayoutEntry("materialSampler")
                                                .setType(wgpu::SamplerBindingType::Filtering)
                                                .setVisibility(wgpu::ShaderStage::Fragment)
                                                .setBinding(2));
        auto lightsLayout = Graphic::Utils::BindGroupLayout("LightsLayout")
                                .addEntry(Graphic::Utils::BufferBindGroupLayoutEntry("ambientLight")
                                              .setType(wgpu::BufferBindingType::Uniform)
                                              .setMinBindingSize(sizeof(glm::vec3) + sizeof(float) /*padding*/)
                                              .setVisibility(wgpu::ShaderStage::Fragment)
                                              .setBinding(0))
                                .addEntry(Graphic::Utils::BufferBindGroupLayoutEntry("pointLights")
                                              .setType(wgpu::BufferBindingType::Uniform)
                                              .setMinBindingSize(Graphic::Resource::PointLightsBuffer::GPUSize())
                                              .setVisibility(wgpu::ShaderStage::Fragment)
                                              .setBinding(1));

        auto vertexLayout = Graphic::Utils::VertexBufferLayout()
                                .addVertexAttribute(wgpu::VertexFormat::Float32x3, 0, 0)
                                .addVertexAttribute(wgpu::VertexFormat::Float32x3, 3 * sizeof(float), 1)
                                .addVertexAttribute(wgpu::VertexFormat::Float32x2, 6 * sizeof(float), 2)
                                .setArrayStride(8 * sizeof(float))
                                .setStepMode(wgpu::VertexStepMode::Vertex);

        auto colorOutput =
            Graphic::Utils::ColorTargetState("END_RENDER_TEXTURE").setFormat(wgpu::TextureFormat::BGRA8UnormSrgb);

        auto depthOutput = Graphic::Utils::DepthStencilState("END_DEPTH_RENDER_TEXTURE")
                               .setFormat(wgpu::TextureFormat::Depth24Plus)
                               .setCompareFunction(wgpu::CompareFunction::Less)
                               .setDepthWriteEnabled(wgpu::OptionalBool::True);

        shaderDescriptor.setShader(DEFAULT_RENDER_PASS_SHADER_CONTENT)
            .setName(DEFAULT_RENDER_PASS_SHADER_NAME)
            .setVertexEntryPoint("vs_main")
            .setFragmentEntryPoint("fs_main")
            .addBindGroupLayout(cameraLayout)
            .addBindGroupLayout(modelLayout)
            .addBindGroupLayout(materialLayout)
            .addBindGroupLayout(lightsLayout)
            .addVertexBufferLayout(vertexLayout)
            .addOutputColorFormat(colorOutput)
            .setOutputDepthFormat(depthOutput);
        const auto validations = shaderDescriptor.validate();
        if (!validations.empty())
        {
            for (const auto &validation : validations)
            {
                if (validation.severity == Utils::ValidationError::Severity::Error)
                {
                    Log::Error(fmt::format("Shader Descriptor Validation Error: {} at {}", validation.message,
                                           validation.location));
                }
                else if (validation.severity == Utils::ValidationError::Severity::Warning)
                {
                    Log::Warn(fmt::format("Shader Descriptor Validation Warning: {} at {}", validation.message,
                                          validation.location));
                }
            }
        }
        return Graphic::Resource::Shader::Create(shaderDescriptor, graphicContext);
    }
};
} // namespace Graphic::Utils
