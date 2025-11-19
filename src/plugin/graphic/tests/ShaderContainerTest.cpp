#include <gtest/gtest.h>

#include "Graphic.hpp"
#include "RenderingPipeline.hpp"

#include <glm/glm.hpp>

const std::string testShaderSource = R"(
struct Uniforms {
    modelViewProjectionMatrix: mat4x4f,
};

@group(0) @binding(0) var<uniform> uniforms: Uniforms;

struct VertexOutput {
  @builtin(position) position: vec4f,
  @location(0) uv: vec2f,
}

@vertex
fn vs_main(
    @location(0) position: vec3f,
    @location(1) uv: vec2f,
) -> VertexOutput {
    var out: VertexOutput;
    out.position = uniforms.modelViewProjectionMatrix * vec4f(position, 1.0);
    out.uv = uv;
    return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f {
    return vec4f(in.uv, 0.0, 1.0);
}
)";

Graphic::Resource::Shader CreateTestShader(const std::string &name, Engine::Core &core)
{
    Graphic::Resource::ShaderDescriptor shaderDescriptor;

    auto vertexLayout = Graphic::Utils::VertexBufferLayout()
                            .addVertexAttribute(wgpu::VertexFormat::Float32x3, 0, 0)
                            .addVertexAttribute(wgpu::VertexFormat::Float32x2, 3 * sizeof(float), 1)
                            .setArrayStride(5 * sizeof(float))
                            .setStepMode(wgpu::VertexStepMode::Vertex);

    auto bindGroupLayout = Graphic::Utils::BindGroupLayout("TestLayout")
                               .addEntry(Graphic::Utils::BufferBindGroupLayoutEntry("BufferEntry")
                                             .setType(wgpu::BufferBindingType::Uniform)
                                             .setHasDynamicOffset(false)
                                             .setMinBindingSize<glm::mat4>()
                                             .setVisibility(wgpu::ShaderStage::Vertex)
                                             .setBinding(0));

    auto colorOutput = Graphic::Utils::ColorTargetState("Color").setFormat(wgpu::TextureFormat::BGRA8Unorm);

    auto depthStencilOutput = Graphic::Utils::DepthStencilState("DepthStencil")
                                  .setFormat(wgpu::TextureFormat::Depth24PlusStencil8)
                                  .setCompareFunction(wgpu::CompareFunction::Less)
                                  .setDepthWriteEnabled(wgpu::OptionalBool::True);

    shaderDescriptor.setShader(testShaderSource)
        .setName(name)
        .setVertexEntryPoint("vs_main")
        .setFragmentEntryPoint("fs_main")
        .addVertexBufferLayout(vertexLayout)
        .addBindGroupLayout(bindGroupLayout)
        .addOutputColorFormat(colorOutput)
        .setOutputDepthFormat(depthStencilOutput);

    return Graphic::Resource::Shader::Create(shaderDescriptor, core.GetResource<Graphic::Resource::Context>());
}

auto TestShaderContainerSystem(Engine::Core &core) -> void
{
    Graphic::Resource::ShaderContainer shaderContainer;

    // Test 1: Add shaders to the container
    entt::hashed_string shader1Id = "test_shader_1";
    entt::hashed_string shader2Id = "test_shader_2";

    auto shader1 = CreateTestShader("TestShader1", core);
    auto shader2 = CreateTestShader("TestShader2", core);

    shaderContainer.Add(shader1Id, std::move(shader1));
    shaderContainer.Add(shader2Id, std::move(shader2));

    EXPECT_TRUE(shaderContainer.Contains(shader1Id));
    EXPECT_TRUE(shaderContainer.Contains(shader2Id));

    auto &retrievedShader1 = shaderContainer.Get(shader1Id);
    auto &retrievedShader2 = shaderContainer.Get(shader2Id);

    EXPECT_EQ(retrievedShader1.getDescriptor().getName(), "TestShader1");
    EXPECT_EQ(retrievedShader2.getDescriptor().getName(), "TestShader2");

    shaderContainer.Remove(shader1Id);
    EXPECT_FALSE(shaderContainer.Contains(shader1Id));
    EXPECT_TRUE(shaderContainer.Contains(shader2Id));

    EXPECT_THROW((void)shaderContainer.Get(shader1Id), Object::ResourceManagerError);
}

TEST(ShaderContainerTest, GlobalRun)
{
    Engine::Core core;

    core.AddPlugins<Graphic::Plugin>();

    core.RegisterSystem<RenderingPipeline::Init>([](Engine::Core &c) {
        c.GetResource<Graphic::Resource::GraphicSettings>()
            .SetWindowSystem(Graphic::Resource::WindowSystem::None)
            .SetOnErrorCallback([](WGPUDevice const *device, WGPUErrorType type, WGPUStringView message,
                                   WGPU_NULLABLE void *userdata1, WGPU_NULLABLE void *userdata2) {
                Log::Error(fmt::format("Custom uncaptured device error: type {:x} ({})", static_cast<uint32_t>(type),
                                       std::string(message.data, message.length)));
                throw std::runtime_error("Custom uncaptured device error occurred");
            })
            .GetWantedLimits()
            .maxBindGroups = 8;
    });

    core.RegisterSystem(TestShaderContainerSystem);

    core.RunSystems();
}
