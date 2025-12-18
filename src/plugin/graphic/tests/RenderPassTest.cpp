#include <gtest/gtest.h>

#include "Graphic.hpp"
#include "RenderingPipeline.hpp"
#include "utils/ConfigureHeadlessGraphics.hpp"
#include "utils/ThrowErrorIfGraphicalErrorHappened.hpp"
#include <glm/gtc/type_ptr.hpp>

using namespace entt::literals;

struct History {
    bool called = false;
};

class SingleExecutionRenderPassTest
    : public Graphic::Resource::ASingleExecutionRenderPass<SingleExecutionRenderPassTest> {
  public:
    explicit SingleExecutionRenderPassTest() : ASingleExecutionRenderPass("TestRenderPass") {}

    void UniqueRenderCallback(wgpu::RenderPassEncoder &renderPass, Engine::Core &core) override
    {
        core.GetResource<History>().called = true;

        renderPass.draw(6, 1, 0, 0);
    }
};

Graphic::Resource::Shader CreateTestShader1(Graphic::Resource::Context &graphicContext)
{
    const std::string shaderSource = R"(

struct Global {
    testValue: vec4f,
};

@group(0) @binding(0) var<uniform> global: Global;


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
    return global.testValue;
}

)";

    Graphic::Resource::ShaderDescriptor shaderDescriptor;

    auto bindGroupLayout = Graphic::Utils::BindGroupLayout("ExampleLayout")
                               .addEntry(Graphic::Utils::BufferBindGroupLayoutEntry("BufferEntry")
                                             .setType(wgpu::BufferBindingType::Uniform)
                                             .setMinBindingSize<glm::vec4>()
                                             .setVisibility(wgpu::ShaderStage::Fragment)
                                             .setBinding(0));
    auto normalColorOutput =
        Graphic::Utils::ColorTargetState("returnTextureTest").setFormat(wgpu::TextureFormat::RGBA8Unorm);

    shaderDescriptor.setShader(shaderSource)
        .setName("ExampleShader")
        .addBindGroupLayout(bindGroupLayout)
        .addOutputColorFormat(normalColorOutput);

    auto errors = shaderDescriptor.validate();
    for (const auto &error : errors)
    {
        if (error.severity == Graphic::Utils::ValidationError::Severity::Error)
            Log::Error(error.message);
    }
    for (const auto &error : errors)
    {
        if (error.severity == Graphic::Utils::ValidationError::Severity::Error)
            throw std::runtime_error("ShaderDescriptor validation failed");
    }

    return Graphic::Resource::Shader::Create(shaderDescriptor, graphicContext);
}

class TestGPUBuffer final : public Graphic::Resource::AGPUBuffer {
  public:
    explicit TestGPUBuffer(std::string label, glm::vec4 value) : _label(std::move(label)), _value(value) {}

    void Create(Engine::Core &core) override
    {
        wgpu::BufferDescriptor bufferDesc(wgpu::Default);
        bufferDesc.label = wgpu::StringView(_label);
        bufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform;
        bufferDesc.size = sizeof(glm::vec4);

        _buffer = core.GetResource<Graphic::Resource::Context>().deviceContext.GetDevice()->createBuffer(bufferDesc);

        core.GetResource<Graphic::Resource::Context>().queue->writeBuffer(_buffer, 0, glm::value_ptr(_value),
                                                                          bufferDesc.size);
        _isCreated = true;
    }
    void Destroy(Engine::Core &) override
    {
        if (_isCreated)
        {
            _isCreated = false;
            _buffer.release();
        }
    }
    bool IsCreated(Engine::Core &) const override { return _isCreated; }
    void Update(Engine::Core &) override {}
    const wgpu::Buffer &GetBuffer() const override
    {
        if (_isCreated == false)
            throw std::runtime_error("Trying to access a GPU buffer that is not created");
        return _buffer;
    }

    const std::string &Label() const { return _label; }

  private:
    glm::vec4 _value;
    bool _isCreated = false;
    std::string _label;
    wgpu::Buffer _buffer;
};

void TestSystem(Engine::Core &core)
{
    auto &context = core.GetResource<Graphic::Resource::Context>();

    SingleExecutionRenderPassTest renderPass{};

    auto shader = CreateTestShader1(context);
    core.GetResource<Graphic::Resource::ShaderContainer>().Add("DefaultTestShader"_hs, std::move(shader));

    core.GetResource<Graphic::Resource::GPUBufferContainer>().Add(
        "TestGPUBuffer1"_hs, std::make_unique<TestGPUBuffer>("TestGPUBuffer1", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)));
    core.GetResource<Graphic::Resource::GPUBufferContainer>().Get("TestGPUBuffer1"_hs)->Create(core);

    Graphic::Resource::BindGroup inputBindGroup(core, "DefaultTestShader"_hs, 0,
                                                {
                                                    {.binding = 0,
                                                     .type = Graphic::Resource::BindGroup::Asset::Type::Buffer,
                                                     .name = "TestGPUBuffer1"_hs,
                                                     .size = sizeof(glm::vec4)},
    });
    core.GetResource<Graphic::Resource::BindGroupManager>().Add("TestBindGroup1"_hs, std::move(inputBindGroup));

    core.GetResource<Graphic::Resource::TextureContainer>().Add(
        "returnTextureTest"_hs, context, "returnTextureTest",
        Graphic::Resource::Image(glm::uvec2(256, 256), [](glm::uvec2) { return glm::u8vec4(255, 0, 0, 255); }));

    Graphic::Resource::ColorOutput colorOutput;
    colorOutput.textureId = "returnTextureTest"_hs;

    renderPass.BindShader("DefaultTestShader");
    renderPass.AddInput(0, "TestBindGroup1");
    renderPass.AddOutput(0, colorOutput);

    EXPECT_NO_THROW(renderPass.Execute(core));

    auto validationErrors = renderPass.validate(core);

    if (!validationErrors.empty())
    {
        for (const auto &error : validationErrors)
        {
            std::cout << error << std::endl;
        }
        FAIL() << "RenderPass validation failed with errors.";
    }

    auto image =
        core.GetResource<Graphic::Resource::TextureContainer>().Get("returnTextureTest"_hs).RetrieveImage(context);

    EXPECT_EQ(image.width, 256);
    EXPECT_EQ(image.height, 256);
    for (const auto &pixel : image.pixels)
    {
        EXPECT_EQ(pixel, glm::u8vec4(0, 255, 0, 255));
    }

    // Uncomment this to check if the retrieved texture data is correct
    // image.ToPng("RenderPassTestOutput.png");

    EXPECT_TRUE(core.GetResource<History>().called);
}

TEST(RenderPass, SingleExecutionTest)
{
    Engine::Core core;

    core.AddPlugins<Graphic::Plugin>();

    core.SetErrorPolicyForAllSchedulers(Engine::Scheduler::SchedulerErrorPolicy::Nothing);

    core.RegisterSystem<RenderingPipeline::Init>(Graphic::Tests::Utils::ConfigureHeadlessGraphics,
                                                 Graphic::Tests::Utils::ThrowErrorIfGraphicalErrorHappened);

    core.RegisterResource(History{});

    core.RegisterSystem(TestSystem);

    EXPECT_NO_THROW(core.RunSystems());
}
