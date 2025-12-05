#include "utils/CreateDefaultTestShader.hpp"
#include "Graphic.hpp"
#include "core/Core.hpp"
#include <glm/mat4x4.hpp>

namespace Graphic::Tests::Utils {
Graphic::Resource::Shader CreateDefaultTestShader(Graphic::Resource::Context &graphicContext)
{
    const std::string shaderSource = R"(
struct Uniforms {
    modelViewProjectionMatrix: mat4x4f,
};

@group(0) @binding(0) var<uniform> uniforms: Uniforms;
@group(0) @binding(1) var cubemapTexture: texture_cube<f32>;
@group(0) @binding(2) var cubemapSampler: sampler;

struct VertexOutput {
  @builtin(position) position: vec4f,
  @location(0) uv: vec2f,
  @location(1) fragPosition: vec3f,
}

@vertex
fn vs_main(
    @location(0) position: vec3f,
    @location(1) uv: vec2f,
) -> VertexOutput {
    var out: VertexOutput;
    out.position = uniforms.modelViewProjectionMatrix * vec4f(position, 1.0);
    out.uv = uv;
    out.fragPosition = 0.5 * (position + vec3(1.0, 1.0, 1.0));
    return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f {
    var cubemapVec = in.fragPosition - vec3(0.5);
    cubemapVec.z *= -1;
    return textureSample(cubemapTexture, cubemapSampler, cubemapVec);

}

)";

    Graphic::Resource::ShaderDescriptor shaderDescriptor;

    auto vertexLayout =
        Graphic::Utils::VertexBufferLayout()
            .addVertexAttribute(wgpu::VertexFormat::Float32x3, 0, 0)
            .addVertexAttribute(wgpu::VertexFormat::Float32x2, 3 * sizeof(float), 1)
            .setArrayStride(
                5 * sizeof(float)) // This can be omitted to test automatic stride calculation (but this is unsafe)
            .setStepMode(wgpu::VertexStepMode::Vertex); // This is done by default
    auto bindGroupLayout =
        Graphic::Utils::BindGroupLayout("ExampleLayout")
            .addEntry(
                Graphic::Utils::BufferBindGroupLayoutEntry("BufferEntry")
                    .setType(wgpu::BufferBindingType::Uniform)
                    .setHasDynamicOffset(false)     // This is done by default
                    .setMinBindingSize<glm::mat4>() // with will try to compute size with types and align to 16 bytes
                    .setMinBindingSize(sizeof(glm::mat4)) // or manually set size, N.B.: you can also dont set it at all
                                                          // and webgpu will try to set it by itself
                    .setVisibility(wgpu::ShaderStage::Vertex)
                    .setBinding(0))
            .addEntry(Graphic::Utils::TextureBindGroupLayoutEntry("TextureEntry")
                          .setSampleType(wgpu::TextureSampleType::Float)
                          .setViewDimension(wgpu::TextureViewDimension::Cube)
                          .setVisibility(wgpu::ShaderStage::Fragment)
                          .setBinding(1))
            .addEntry(Graphic::Utils::SamplerBindGroupLayoutEntry("SamplerEntry")
                          .setSamplerType(wgpu::SamplerBindingType::Filtering)
                          .setVisibility(wgpu::ShaderStage::Fragment)
                          .setBinding(2));
    auto normalColorOutput = Graphic::Utils::ColorTargetState("NormalColor").setFormat(wgpu::TextureFormat::BGRA8Unorm);
    auto albedoColorOutput = Graphic::Utils::ColorTargetState("AlbedoColor").setFormat(wgpu::TextureFormat::BGRA8Unorm);
    auto depthStencilOutput = Graphic::Utils::DepthStencilState("DepthStencil")
                                  .setFormat(wgpu::TextureFormat::Depth24PlusStencil8)
                                  .setCompareFunction(wgpu::CompareFunction::Less)
                                  .setDepthWriteEnabled(wgpu::OptionalBool::True);

    shaderDescriptor.setShader(shaderSource)
        .setName("ExampleShader")
        .setVertexEntryPoint("vs_main")   // This is done by default
        .setFragmentEntryPoint("fs_main") // This is done by default
        .addVertexBufferLayout(vertexLayout)
        .addBindGroupLayout(bindGroupLayout)
        .addOutputColorFormat(normalColorOutput)
        .addOutputColorFormat(albedoColorOutput)
        .setOutputDepthFormat(depthStencilOutput)
        .setCullMode(wgpu::CullMode::Back)                            // This is done by default
        .setPrimitiveTopology(wgpu::PrimitiveTopology::TriangleList); // This is done by default

    auto errors = shaderDescriptor.validate();
    for (const auto &error : errors)
    {
        std::cout << error << std::endl;
    }

    return Graphic::Resource::Shader::Create(shaderDescriptor, graphicContext);
}
} // namespace Graphic::Tests::Utils
