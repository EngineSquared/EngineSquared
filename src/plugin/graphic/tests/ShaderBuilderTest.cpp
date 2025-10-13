#include <gtest/gtest.h>

#include "Graphic.hpp"
#include <glm/glm.hpp>

TEST(GraphicPlugin, GlobalRun)
{
    Plugin::Graphic::Utils::ShaderBuilder builder;

    auto vertexLayout = Plugin::Graphic::Utils::VertexBufferLayout()
        .addVertexAttribute(wgpu::VertexFormat::Float32x3, 0, 0)
        .addVertexAttribute(wgpu::VertexFormat::Float32x2, 3 * sizeof(float), 1)
        .setArrayStride(5 * sizeof(float)) // This can be omitted to test automatic stride calculation (unsafe)
        .setStepMode(wgpu::VertexStepMode::Vertex); // This is done by default
    auto bindGroupLayout = Plugin::Graphic::Utils::BindGroupLayout("ExampleLayout")
        .addEntry(Plugin::Graphic::Utils::BufferBindGroupLayoutEntry("BufferEntry")
            .setHasDynamicOffset(false) // This is done by default
            .setType<glm::mat4>()
            .setVisibility(wgpu::ShaderStage::Vertex)
            .setBinding(0))
        .addEntry(Plugin::Graphic::Utils::TextureBindGroupLayoutEntry("TextureEntry")
            .setSampleType(wgpu::TextureSampleType::Float)
            .setViewDimension(wgpu::TextureViewDimension::Cube)
            .setVisibility(wgpu::ShaderStage::Fragment)
            .setBinding(1))
        .addEntry(Plugin::Graphic::Utils::SamplerBindGroupLayoutEntry("SamplerEntry")
            .setSamplerType(wgpu::SamplerBindingType::Filtering)
            .setVisibility(wgpu::ShaderStage::Fragment)
            .setBinding(2));
    auto normalColorOutput = Plugin::Graphic::Utils::ColorTargetState("NormalColor")
        .setFormat(wgpu::TextureFormat::BGRA8Unorm);
    auto albedoColorOutput = Plugin::Graphic::Utils::ColorTargetState("AlbedoColor")
        .setFormat(wgpu::TextureFormat::BGRA8Unorm);
    auto depthStencilOutput = Plugin::Graphic::Utils::DepthStencilState("DepthStencil")
        .setFormat(wgpu::TextureFormat::Depth24PlusStencil8)
        .setCompareFunction(wgpu::CompareFunction::Less)
        .setDepthWriteEnabled(wgpu::OptionalBool::True);

    builder.setShader("void vs_main() { } void fs_main() { }")
        .setVertexEntryPoint("vs_main") // This is done by default
        .setFragmentEntryPoint("fs_main") // This is done by default
        .addVertexBufferLayout(vertexLayout)
        .addBindGroupLayout(bindGroupLayout)
        .addOutputColorFormat(normalColorOutput)
        .addOutputColorFormat(albedoColorOutput)
        .setOutputDepthFormat(depthStencilOutput)
        .setCullMode(wgpu::CullMode::Back) // This is done by default
        .setPrimitiveTopology(wgpu::PrimitiveTopology::TriangleList); // This is done by default

    auto errors = builder.validate();
    for (const auto &error : errors)
    {
        std::cout << error << std::endl;
    }
}
