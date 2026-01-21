#pragma once

#include "Logger.hpp"
#include "component/GPUCamera.hpp"
#include "component/GPUMaterial.hpp"
#include "component/GPUMesh.hpp"
#include "component/GPUTransform.hpp"
#include "core/Core.hpp"
#include "entity/Entity.hpp"
#include "resource/ASingleExecutionRenderPass.hpp"
#include "utils/DefaultMaterial.hpp"
#include "utils/shader/BufferBindGroupLayoutEntry.hpp"
#include "utils/shader/SamplerBindGroupLayoutEntry.hpp"
#include "utils/shader/TextureBindGroupLayoutEntry.hpp"
#include <entt/core/hashed_string.hpp>
#include <string_view>

namespace DefaultPipeline::Resource {
static inline constexpr std::string_view GBUFFER_PASS_OUTPUT_NORMAL = "GBUFFER_PASS_OUTPUT_NORMAL";
static inline const entt::hashed_string GBUFFER_PASS_OUTPUT_NORMAL_ID{GBUFFER_PASS_OUTPUT_NORMAL.data(),
                                                                      GBUFFER_PASS_OUTPUT_NORMAL.size()};
static inline constexpr std::string_view GBUFFER_PASS_OUTPUT_ALBEDO = "GBUFFER_PASS_OUTPUT_ALBEDO";
static inline const entt::hashed_string GBUFFER_PASS_OUTPUT_ALBEDO_ID{GBUFFER_PASS_OUTPUT_ALBEDO.data(),
                                                                      GBUFFER_PASS_OUTPUT_ALBEDO.size()};
static inline constexpr std::string_view GBUFFER_PASS_OUTPUT_DEPTH = "GBUFFER_PASS_OUTPUT_DEPTH";
static inline const entt::hashed_string GBUFFER_PASS_OUTPUT_DEPTH_ID{GBUFFER_PASS_OUTPUT_DEPTH.data(),
                                                                     GBUFFER_PASS_OUTPUT_DEPTH.size()};
static inline constexpr std::string_view GBUFFER_PASS_NAME = "GBUFFER_PASS_NAME";
static inline constexpr std::string_view GBUFFER_SHADER_NAME = "GBUFFER_SHADER_NAME";
static inline const entt::hashed_string GBUFFER_SHADER_ID =
    entt::hashed_string{GBUFFER_SHADER_NAME.data(), GBUFFER_SHADER_NAME.size()};
static inline constexpr std::string_view GBUFFER_SHADE_CONTENT = R"(
struct Camera {
  viewProjectionMatrix : mat4x4<f32>,
}

struct Object {
  model : mat4x4<f32>,
  normal : mat4x4<f32>,
}

struct Material {
    emission: vec3f,
    padding: f32,
};

struct VertexToFragment {
  @builtin(position) Position : vec4f,
  @location(0) fragNormal: vec3f,
  @location(1) fragUV: vec2f,
}

struct GBufferOutput {
    @location(0) normal : vec4f,
    @location(1) albedo : vec4f,
}

@group(0) @binding(0) var<uniform> camera: Camera;

@group(1) @binding(0) var<uniform> object: Object;

@group(2) @binding(0) var<uniform> material : Material;
@group(2) @binding(1) var texture : texture_2d<f32>;
@group(2) @binding(2) var textureSampler : sampler;

@vertex
fn vs_main(
  @location(0) position: vec3f,
  @location(1) normal: vec3f,
  @location(2) uv: vec2f,
) -> VertexToFragment {
  var output : VertexToFragment;
  let worldPosition = (object.model * vec4(position, 1.0)).xyz;
  output.Position = camera.viewProjectionMatrix * vec4(worldPosition, 1.0);
  output.fragNormal = normalize((object.normal * vec4(normal, 1.0)).xyz);
  output.fragUV = uv;
  return output;
}

@fragment
fn fs_main(
  @location(0) fragNormal: vec3f,
  @location(1) fragUV : vec2f
) -> GBufferOutput {
  var output : GBufferOutput;
  output.normal = vec4(normalize(fragNormal), 1.0);
  output.albedo = vec4(textureSample(texture, textureSampler, fragUV).rgb, 1.0);

  return output;
}

)";

class GBuffer : public Graphic::Resource::ASingleExecutionRenderPass<GBuffer> {
  public:
    explicit GBuffer(std::string_view name = GBUFFER_PASS_NAME) : ASingleExecutionRenderPass<GBuffer>(name) {}

    void UniqueRenderCallback(wgpu::RenderPassEncoder &renderPass, Engine::Core &core) override
    {
        const auto &bindGroupManager = core.GetResource<Graphic::Resource::BindGroupManager>();
        const auto &bufferContainer = core.GetResource<Graphic::Resource::GPUBufferContainer>();

        auto cameraView = core.GetRegistry().view<Component::GPUCamera>();
        if (cameraView.empty())
        {
            Log::Error("GBuffer::UniqueRenderCallback: No camera with GPUCamera component found.");
            return;
        }
        Engine::Entity camera{cameraView.front()};
        const auto &cameraGPUComponent = camera.GetComponents<Component::GPUCamera>(core);

        const auto &cameraBindGroup = bindGroupManager.Get(cameraGPUComponent.bindGroup);
        renderPass.setBindGroup(0, cameraBindGroup.GetBindGroup(), 0, nullptr);

        auto view = core.GetRegistry().view<Component::GPUTransform, Component::GPUMesh>();

        for (auto &&[e, transform, gpuMesh] : view.each())
        {
            Engine::Entity entity{e};

            const auto &transformBindGroup = bindGroupManager.Get(transform.bindGroup);
            renderPass.setBindGroup(transformBindGroup.GetLayoutIndex(), transformBindGroup.GetBindGroup(), 0, nullptr);

            entt::hashed_string gpuMaterialId{};
            if (entity.HasComponents<Component::GPUMaterial>(core))
            {
                const auto &materialComponent = entity.GetComponents<Component::GPUMaterial>(core);
                gpuMaterialId = materialComponent.bindGroup;
            }
            else
            {
                gpuMaterialId = Utils::DEFAULT_MATERIAL_BIND_GROUP_ID;
            }
            const auto &materialBindGroup = bindGroupManager.Get(gpuMaterialId);
            renderPass.setBindGroup(materialBindGroup.GetLayoutIndex(), materialBindGroup.GetBindGroup(), 0, nullptr);

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

        auto cameraLayout =
            Graphic::Utils::BindGroupLayout("Camera").addEntry(Graphic::Utils::BufferBindGroupLayoutEntry("camera")
                                                                   .setType(wgpu::BufferBindingType::Uniform)
                                                                   .setMinBindingSize(sizeof(glm::mat4))
                                                                   .setVisibility(wgpu::ShaderStage::Vertex)
                                                                   .setBinding(0));
        // Model buffer contains: mat4 modelMatrix (64 bytes) + 3 * vec4 normalMatrix columns (48 bytes) = 112 bytes
        auto modelLayout = Graphic::Utils::BindGroupLayout("Model").addEntry(
            Graphic::Utils::BufferBindGroupLayoutEntry("model")
                .setType(wgpu::BufferBindingType::Uniform)
                .setMinBindingSize(sizeof(glm::mat4) + sizeof(glm::mat4))
                .setVisibility(wgpu::ShaderStage::Vertex)
                .setBinding(0));
        auto materialLayout = Graphic::Utils::BindGroupLayout("Material")
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

        auto vertexLayout = Graphic::Utils::VertexBufferLayout()
                                .addVertexAttribute(wgpu::VertexFormat::Float32x3, 0, 0)
                                .addVertexAttribute(wgpu::VertexFormat::Float32x3, 3 * sizeof(float), 1)
                                .addVertexAttribute(wgpu::VertexFormat::Float32x2, 6 * sizeof(float), 2)
                                .setArrayStride(8 * sizeof(float))
                                .setStepMode(wgpu::VertexStepMode::Vertex);

        auto normalOutput =
            Graphic::Utils::ColorTargetState("GBUFFER_NORMAL").setFormat(wgpu::TextureFormat::RGBA16Float);

        auto albedoOutput =
            Graphic::Utils::ColorTargetState("GBUFFER_ALBEDO").setFormat(wgpu::TextureFormat::BGRA8Unorm);

        auto depthOutput = Graphic::Utils::DepthStencilState("GBUFFER_DEPTH")
                               .setFormat(wgpu::TextureFormat::Depth32Float)
                               .setCompareFunction(wgpu::CompareFunction::Less)
                               .setDepthWriteEnabled(wgpu::OptionalBool::True);

        shaderDescriptor.setShader(GBUFFER_SHADE_CONTENT)
            .setName(GBUFFER_SHADER_NAME)
            .setVertexEntryPoint("vs_main")
            .setFragmentEntryPoint("fs_main")
            .addBindGroupLayout(cameraLayout)
            .addBindGroupLayout(modelLayout)
            .addBindGroupLayout(materialLayout)
            .addVertexBufferLayout(vertexLayout)
            .addOutputColorFormat(normalOutput)
            .addOutputColorFormat(albedoOutput)
            .setOutputDepthFormat(depthOutput);
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
