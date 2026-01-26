#pragma once

#include "Logger.hpp"
#include "component/GPUCamera.hpp"
#include "component/GPUMaterial.hpp"
#include "component/GPUMesh.hpp"
#include "component/GPUTransform.hpp"
#include "core/Core.hpp"
#include "entity/Entity.hpp"
#include "resource/AMultipleExecutionRenderPass.hpp"
#include "resource/buffer/CameraGPUBuffer.hpp"
#include "resource/buffer/DirectionalLightBuffer.hpp"
#include "resource/buffer/PointLightsBuffer.hpp"
#include "utils/DefaultMaterial.hpp"
#include "utils/PointLights.hpp"
#include "utils/shader/BufferBindGroupLayoutEntry.hpp"
#include "utils/shader/SamplerBindGroupLayoutEntry.hpp"
#include "utils/shader/TextureBindGroupLayoutEntry.hpp"
#include <entt/core/hashed_string.hpp>
#include <string_view>

namespace DefaultPipeline::Resource {
static inline constexpr std::string_view SHADOW_PASS_OUTPUT = "SHADOW_PASS_OUTPUT";
static inline const entt::hashed_string SHADOW_PASS_OUTPUT_ID{SHADOW_PASS_OUTPUT.data(), SHADOW_PASS_OUTPUT.size()};
static inline constexpr std::string_view SHADOW_PASS_NAME = "SHADOW_PASS";
static inline const entt::hashed_string SHADOW_PASS_ID{SHADOW_PASS_NAME.data(), SHADOW_PASS_NAME.size()};
static inline constexpr std::string_view SHADOW_SHADER_NAME = "SHADOW_SHADER";
static inline const entt::hashed_string SHADOW_SHADER_ID =
    entt::hashed_string{SHADOW_SHADER_NAME.data(), SHADOW_SHADER_NAME.size()};

static inline constexpr std::string_view SHADOW_BINDGROUP_TEXTURES_NAME = "SHADOW_BINDGROUP_TEXTURES";
static inline const entt::hashed_string SHADOW_BINDGROUP_TEXTURES_ID =
    entt::hashed_string{SHADOW_BINDGROUP_TEXTURES_NAME.data(), SHADOW_BINDGROUP_TEXTURES_NAME.size()};

static inline constexpr std::string_view SHADOW_SHADER_CONTENT = R"(
const MAX_POINT_LIGHTS: u32 = 64u;

struct Input {
    @location(0) position: vec3f,
    @location(1) normal: vec3f,
    @location(2) uv: vec2f,
};

struct Object {
  model : mat4x4<f32>,
  normal : mat4x4<f32>,
}

struct Light {
  viewProjection: mat4x4f,
};

@group(0) @binding(0) var<uniform> light: Light;
@group(1) @binding(0) var<uniform> object: Object;

@vertex
fn vs_main(
    input : Input
) -> @builtin(position) vec4f {
    return light.viewProjection * object.model * vec4f(input.position, 1.0);
}

@fragment
fn fs_main() {}
)";

class Shadow : public Graphic::Resource::AMultipleExecutionRenderPass<Shadow> {
  public:
    explicit Shadow(std::string_view name = SHADOW_PASS_NAME) : AMultipleExecutionRenderPass<Shadow>(name) {}

    uint16_t GetNumberOfPasses(Engine::Core &core) override
    {
        uint16_t count = 0;
        core.GetRegistry().view<Component::GPUDirectionalLight>().each(
            [&count](auto, const auto &) { count++; });
        return std::min(count, static_cast<uint16_t>(1));
    }

    void perPass(uint16_t passIndex, Engine::Core &core) override
    {
        auto view = core.GetRegistry().view<Component::GPUDirectionalLight>();
        if (view.empty())
        {
            return;
        }
        uint16_t i = 0;
        for (auto &&[e, light] : view.each())
        {
            Engine::Entity entity{core, e};
            if (i == passIndex)
            {
                auto &lightGPUComponent = entity.GetComponents<Component::GPUDirectionalLight>();
                auto &outputs = this->GetOutputs();
                outputs.depthBuffer->depthTextureView = lightGPUComponent.shadowTextureView;
                lightGPUComponent.shadowTextureIndex = passIndex;
                return;
            }
            i++;
        }
    };

    void UniqueRenderCallback(wgpu::RenderPassEncoder &renderPass, Engine::Core &core) override
    {
        const auto &bindGroupManager = core.GetResource<Graphic::Resource::BindGroupManager>();
        const auto &bufferContainer = core.GetResource<Graphic::Resource::GPUBufferContainer>();

        auto light = core.GetRegistry().view<Component::GPUDirectionalLight>();
        if (light.empty())
        {
            return;
        }
        Engine::Entity lightEntity{core, light.front()};
        const auto &lightGPUComponent = lightEntity.GetComponents<Component::GPUDirectionalLight>();
        const auto &lightBindGroup = bindGroupManager.Get(lightGPUComponent.bindGroupData);
        renderPass.setBindGroup(0, lightBindGroup.GetBindGroup(), 0, nullptr);

        auto view = core.GetRegistry().view<Component::GPUTransform, Component::GPUMesh>();

        for (auto &&[e, transform, gpuMesh] : view.each())
        {
            Engine::Entity entity{core, e};

            const auto &transformBindGroup = bindGroupManager.Get(transform.bindGroup);
            renderPass.setBindGroup(1, transformBindGroup.GetBindGroup(), 0, nullptr);
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

        auto lightLayout = Graphic::Utils::BindGroupLayout("light").addEntry(
            Graphic::Utils::BufferBindGroupLayoutEntry("light")
                .setType(wgpu::BufferBindingType::Uniform)
                .setMinBindingSize(Resource::DirectionalLightBuffer::DirectionalLightTransfer::GPUSize())
                .setVisibility(wgpu::ShaderStage::Vertex)
                .setBinding(0));
        auto objectLayout = Graphic::Utils::BindGroupLayout("object").addEntry(
            Graphic::Utils::BufferBindGroupLayoutEntry("model&normal")
                .setType(wgpu::BufferBindingType::Uniform)
                .setMinBindingSize(sizeof(glm::mat4) + sizeof(glm::mat4))
                .setVisibility(wgpu::ShaderStage::Vertex)
                .setBinding(0));

        auto vertexLayout = Graphic::Utils::VertexBufferLayout()
                                .addVertexAttribute(wgpu::VertexFormat::Float32x3, 0, 0)
                                .addVertexAttribute(wgpu::VertexFormat::Float32x3, 3 * sizeof(float), 1)
                                .addVertexAttribute(wgpu::VertexFormat::Float32x2, 6 * sizeof(float), 2)
                                .setArrayStride(8 * sizeof(float))
                                .setStepMode(wgpu::VertexStepMode::Vertex);

        auto depthOutput = Graphic::Utils::DepthStencilState("SHADOW_OUTPUT")
                               .setFormat(wgpu::TextureFormat::Depth32Float)
                               .setCompareFunction(wgpu::CompareFunction::Less)
                               .setDepthWriteEnabled(wgpu::OptionalBool::True);

        shaderDescriptor.setShader(SHADOW_SHADER_CONTENT)
            .setName(SHADOW_SHADER_NAME)
            .addVertexBufferLayout(vertexLayout)
            .setVertexEntryPoint("vs_main")
            .setFragmentEntryPoint("fs_main")
            .addBindGroupLayout(lightLayout)
            .addBindGroupLayout(objectLayout)
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
