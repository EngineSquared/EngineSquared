#pragma once

#include "component/GPUCamera.hpp"
#include "component/GPUMaterial.hpp"
#include "component/GPUMesh.hpp"
#include "component/GPUTransform.hpp"
#include "component/Transform.hpp"
#include "entity/Entity.hpp"
#include "resource/Shader.hpp"
#include "resource/ShaderDescriptor.hpp"
#include "resource/SingleExecutionRenderPass.hpp"
#include "utils/shader/BufferBindGroupLayoutEntry.hpp"
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

@group(0) @binding(0) var<uniform> camera : Camera;
@group(1) @binding(0) var<uniform> model : Model;
@group(2) @binding(0) var<uniform> material : Material;

struct VertexInput {
    @location(0) position : vec3f,
    @location(1) normal : vec3f,
    @location(2) uv : vec2f,
};

struct VertexOutput {
    @builtin(position) Position : vec4f,
    @location(0) fragUV : vec2f,
};

@vertex
fn vs_main(
    input : VertexInput
) -> VertexOutput {
    var output : VertexOutput;
    output.Position = camera.viewProjectionMatrix * model.modelMatrix * vec4f(input.position, 1.0);
    output.fragUV = input.uv;
    return output;
}

@fragment
fn fs_main(
    input : VertexOutput
) -> @location(0) vec4f {
    var color : vec4f = vec4f(material.emission, 1.0);
    return color;
}

)";

class DefaultRenderPass : public Graphic::Resource::ASingleExecutionRenderPass<DefaultRenderPass> {
  public:
    explicit DefaultRenderPass(void) : ASingleExecutionRenderPass(DEFAULT_RENDER_PASS_NAME) {}

    void UniqueRenderCallback(wgpu::RenderPassEncoder &renderPass, Engine::Core &core) override
    {
        Engine::Entity camera(core.GetRegistry().view<Graphic::Component::GPUCamera>().front());
        auto &cameraGPUComponent = camera.GetComponents<Graphic::Component::GPUCamera>(core);

        auto &cameraBindGroup =
            core.GetResource<Graphic::Resource::BindGroupManager>().Get(cameraGPUComponent.bindGroup);

        renderPass.setBindGroup(0, cameraBindGroup.GetBindGroup(), 0, nullptr);

        const auto &bufferContainer = core.GetResource<Graphic::Resource::GPUBufferContainer>();
        const auto &bindgroupContainer = core.GetResource<Graphic::Resource::BindGroupManager>();

        auto view = core.GetRegistry().view<Graphic::Component::GPUTransform, Graphic::Component::GPUMesh>();

        for (auto &&[e, transform, gpuMesh] : view.each())
        {
            Engine::Entity entity(e);

            const auto &transformBindgroup = bindgroupContainer.Get(transform.bindGroup);
            renderPass.setBindGroup(transformBindgroup.GetLayoutIndex(), transformBindgroup.GetBindGroup(), 0,
                                    nullptr);

            entt::hashed_string gpuMaterialId{};
            if (entity.HasComponents<Graphic::Component::GPUMaterial>(core))
            {
                const auto &materialComponent = entity.GetComponents<Graphic::Component::GPUMaterial>(core);
                gpuMaterialId = materialComponent.bindGroup;
            }
            else {
                std::string bindGroupName = "DEFAULT_MATERIAL_BIND_GROUP";
                entt::hashed_string bindGroupId{bindGroupName.data(), bindGroupName.size()};
                gpuMaterialId = bindGroupId;
            }
            const auto &materialBindgroup = bindgroupContainer.Get(gpuMaterialId);
            renderPass.setBindGroup(materialBindgroup.GetLayoutIndex(), materialBindgroup.GetBindGroup(), 0,
                                    nullptr);

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
                                                .setBinding(0));

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
