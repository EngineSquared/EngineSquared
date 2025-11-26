#include <gtest/gtest.h>

#include "Graphic.hpp"
#include "RenderingPipeline.hpp"

#include <glm/glm.hpp>

namespace {

const std::string bindGroupShaderSource = R"(
// Texture
@group(0) @binding(0) var testTexture: texture_2d<f32>;
// Buffer
@group(0) @binding(1) var<storage, read> testBuffer: array<f32>;
// Sampler
@group(0) @binding(2) var testSampler: sampler;

struct VertexOutput {
  @builtin(position) position: vec4f,
};

@vertex
fn vs_main(@location(0) position: vec3f) -> VertexOutput {
    var output: VertexOutput;
    output.position = vec4f(position, 1.0);
    return output;
}

@fragment
fn fs_main(input: VertexOutput) -> @location(0) vec4f {
    let texColor = textureSample(testTexture, testSampler, vec2f(0.0, 0.0));
    let bufferValue = testBuffer[0];
    return vec4f(texColor.r + bufferValue, texColor.g, texColor.b, texColor.a);
}
)";

class ArrayOfFloatGPUBuffer final : public Graphic::Resource::AGPUBuffer {
  public:
    explicit ArrayOfFloatGPUBuffer(std::vector<float> &&data) : _data(std::move(data)) {}

    ~ArrayOfFloatGPUBuffer() override { this->Destroy(); }

    ArrayOfFloatGPUBuffer(const ArrayOfFloatGPUBuffer &) = delete;
    ArrayOfFloatGPUBuffer &operator=(const ArrayOfFloatGPUBuffer &) = delete;
    ArrayOfFloatGPUBuffer(ArrayOfFloatGPUBuffer &&) = default;
    ArrayOfFloatGPUBuffer &operator=(ArrayOfFloatGPUBuffer &&) = default;

    void Create(Engine::Core &core) override
    {
        wgpu::BufferDescriptor bufferDescriptor;
        std::string label = "ArrayOfFloatGPUBuffer";
        bufferDescriptor.label = wgpu::StringView(label);
        bufferDescriptor.size = sizeof(float) * _data.size();
        bufferDescriptor.usage = wgpu::BufferUsage::Storage | wgpu::BufferUsage::CopyDst;

        _buffer =
            core.GetResource<Graphic::Resource::Context>().deviceContext.GetDevice()->createBuffer(bufferDescriptor);

        if (!_buffer)
            throw std::runtime_error("Failed to create GPU Buffer");

        core.GetResource<Graphic::Resource::Context>().queue->writeBuffer(_buffer.value(), 0, _data.data(),
                                                                          sizeof(float) * _data.size());
    }
    void Destroy(Engine::Core &) override { Destroy(); }

    void Destroy(void) { _buffer.reset(); }

    bool IsCreated(Engine::Core &) const override { return _buffer.has_value(); }
    void Update(Engine::Core &) override {}
    const wgpu::Buffer &GetBuffer() const override
    {
        if (!_buffer.has_value())
            Log::Error("ArrayOfFloatGPUBuffer is not created");
        return _buffer.value();
    }

  private:
    std::vector<float> _data;
    std::optional<wgpu::Buffer> _buffer = nullptr;
};

Graphic::Resource::Shader CreateShader(Engine::Core &core)
{
    Graphic::Resource::ShaderDescriptor shaderDescriptor;

    auto vertexLayout = Graphic::Utils::VertexBufferLayout()
                            .addVertexAttribute(wgpu::VertexFormat::Float32x3, 0, 0)
                            .setArrayStride(3 * sizeof(float))
                            .setStepMode(wgpu::VertexStepMode::Vertex);

    auto bindGroupLayout = Graphic::Utils::BindGroupLayout("BindGroupTextureLayout")
                               .addEntry(Graphic::Utils::TextureBindGroupLayoutEntry("TextureEntry")
                                             .setSampleType(wgpu::TextureSampleType::Float)
                                             .setViewDimension(wgpu::TextureViewDimension::_2D)
                                             .setVisibility(wgpu::ShaderStage::Fragment)
                                             .setBinding(0))
                               .addEntry(Graphic::Utils::BufferBindGroupLayoutEntry("BufferEntry")
                                             .setMinBindingSize<float>()
                                             .setType(wgpu::BufferBindingType::ReadOnlyStorage)
                                             .setVisibility(wgpu::ShaderStage::Fragment)
                                             .setBinding(1))
                               .addEntry(Graphic::Utils::SamplerBindGroupLayoutEntry("SamplerEntry")
                                             .setSamplerType(wgpu::SamplerBindingType::Filtering)
                                             .setVisibility(wgpu::ShaderStage::Fragment)
                                             .setBinding(2));

    auto colorOutput = Graphic::Utils::ColorTargetState("Color").setFormat(wgpu::TextureFormat::BGRA8Unorm);
    auto depthState = Graphic::Utils::DepthStencilState("Depth")
                          .setFormat(wgpu::TextureFormat::Depth24PlusStencil8)
                          .setCompareFunction(wgpu::CompareFunction::Less)
                          .setDepthWriteEnabled(wgpu::OptionalBool::False);

    shaderDescriptor.setShader(bindGroupShaderSource)
        .setName("BindGroupShader")
        .setVertexEntryPoint("vs_main")
        .setFragmentEntryPoint("fs_main")
        .addVertexBufferLayout(vertexLayout)
        .addBindGroupLayout(bindGroupLayout)
        .addOutputColorFormat(colorOutput)
        .setOutputDepthFormat(depthState);

    return Graphic::Resource::Shader::Create(shaderDescriptor, core.GetResource<Graphic::Resource::Context>());
}

void ConfigureHeadlessGraphics(Engine::Core &core)
{
    core.GetResource<Graphic::Resource::GraphicSettings>().SetWindowSystem(Graphic::Resource::WindowSystem::None);
}

void ThrowErrorIfGraphicalErrorHappened(Engine::Core &core)
{
    core.GetResource<Graphic::Resource::GraphicSettings>().SetOnErrorCallback(
        [](WGPUDevice const *device, WGPUErrorType type, WGPUStringView message, WGPU_NULLABLE void *userdata1,
           WGPU_NULLABLE void *userdata2) {
            Log::Error(fmt::format("Custom uncaptured device error: type {:x} ({})", static_cast<uint32_t>(type),
                                   std::string(message.data, message.length)));
            throw std::runtime_error("Custom uncaptured device error occurred");
        });
}
} // namespace

Graphic::Resource::BindGroup CreateBindGroup(Engine::Core &core)
{
    auto shaderId = entt::hashed_string("bindgroup_texture_shader");
    auto textureId = entt::hashed_string("bindgroup_texture_asset");
    auto bufferId = entt::hashed_string("bindgroup_buffer_asset");
    auto samplerId = entt::hashed_string("bindgroup_sampler_asset");

    core.GetResource<Graphic::Resource::ShaderContainer>().Add(shaderId, CreateShader(core));

    { // Create texture asset
        auto &context = core.GetResource<Graphic::Resource::Context>();
        auto &textures = core.GetResource<Graphic::Resource::TextureContainer>();
        auto image = Graphic::Resource::Image(glm::uvec2(2, 2), [](glm::uvec2) { return glm::u8vec4(255, 0, 0, 255); });
        textures.Add(textureId, context, "BindGroupTextureA", image);
    }

    { // Create buffer asset
        auto &gpuBuffers = core.GetResource<Graphic::Resource::GPUBufferContainer>();
        gpuBuffers.Add(bufferId, std::make_unique<ArrayOfFloatGPUBuffer>(std::vector<float>{0.5f}));
        gpuBuffers.Get(bufferId)->Create(core);
    }
    { // Create sampler asset
        auto &samplers = core.GetResource<Graphic::Resource::SamplerContainer>();
        auto &device = core.GetResource<Graphic::Resource::Context>().deviceContext.GetDevice().value();
        samplers.Add(samplerId, Graphic::Resource::Sampler(device));
    }

    return Graphic::Resource::BindGroup(
        core, shaderId, 0,
        {
            {0, Graphic::Resource::BindGroup::Asset::Type::Texture, textureId, 0            },
            {1, Graphic::Resource::BindGroup::Asset::Type::Buffer,  bufferId,  sizeof(float)},
            {2, Graphic::Resource::BindGroup::Asset::Type::Sampler, samplerId, 0            }
    });
}

TEST(BindGroupTest, CreatesEntriesForTextureAssets)
{
    Engine::Core core;
    core.AddPlugins<Graphic::Plugin>();

    core.RegisterSystem<RenderingPipeline::Init>(ConfigureHeadlessGraphics, ThrowErrorIfGraphicalErrorHappened);

    core.RegisterSystem([](Engine::Core &core) {
        auto textureId = entt::hashed_string("bindgroup_texture_asset");

        auto bindGroup = CreateBindGroup(core);

        const auto &entries = bindGroup.GetEntries();
        ASSERT_EQ(entries.size(), 2u);
        EXPECT_EQ(entries.at(0).binding, 0u);
        EXPECT_EQ(entries.at(0).textureView,
                  core.GetResource<Graphic::Resource::TextureContainer>().Get(textureId).GetDefaultView());
        EXPECT_EQ(entries.at(1).binding, 1u);
        EXPECT_EQ(entries.at(1).buffer, core.GetResource<Graphic::Resource::GPUBufferContainer>()
                                            .Get(entt::hashed_string("bindgroup_buffer_asset"))
                                            ->GetBuffer());
        EXPECT_EQ(entries.at(2).binding, 2u);
        EXPECT_EQ(entries.at(2).sampler, core.GetResource<Graphic::Resource::SamplerContainer>()
                                             .Get(entt::hashed_string("bindgroup_sampler_asset")).getSampler());
        EXPECT_TRUE(bindGroup.GetBindGroup());
    });

    EXPECT_NO_THROW(core.RunSystems());
}

TEST(BindGroupTest, RefreshUpdatesTextureBindings)
{
    Engine::Core core;
    core.AddPlugins<Graphic::Plugin>();

    core.RegisterSystem<RenderingPipeline::Init>(ConfigureHeadlessGraphics, ThrowErrorIfGraphicalErrorHappened);

    core.RegisterSystem([](Engine::Core &core) {
        auto bindGroup = CreateBindGroup(core);

        auto textureId = entt::hashed_string("bindgroup_texture_asset");
        auto &textures = core.GetResource<Graphic::Resource::TextureContainer>();
        auto &context = core.GetResource<Graphic::Resource::Context>();
        auto initialView = bindGroup.GetEntries().front().textureView;

        textures.Remove(textureId);
        textures.Add(
            textureId, context, "bindgroup_texture_asset_name",
            Graphic::Resource::Image(glm::uvec2(2, 2), [](glm::uvec2) { return glm::u8vec4(200, 100, 50, 255); }));
        auto updatedView = textures.Get(textureId).GetDefaultView();
        EXPECT_NE(bindGroup.GetEntries().at(0).textureView, updatedView);

        bindGroup.Refresh(core);

        EXPECT_EQ(bindGroup.GetEntries().at(0).textureView, updatedView);
    });

    EXPECT_NO_THROW(core.RunSystems());
}

TEST(BindGroupTest, RefreshUpdatesBufferBindings)
{
    Engine::Core core;
    core.AddPlugins<Graphic::Plugin>();

    core.RegisterSystem<RenderingPipeline::Init>(ConfigureHeadlessGraphics, ThrowErrorIfGraphicalErrorHappened);

    core.RegisterSystem([](Engine::Core &core) {
        auto bindGroup = CreateBindGroup(core);

        auto bufferId = entt::hashed_string("bindgroup_buffer_asset");
        auto &gpuBuffers = core.GetResource<Graphic::Resource::GPUBufferContainer>();

        gpuBuffers.Remove(bufferId);
        gpuBuffers.Add(bufferId, std::make_unique<ArrayOfFloatGPUBuffer>(std::vector<float>{1.0f}));
        gpuBuffers.Get(bufferId)->Create(core);
        auto updatedBuffer = gpuBuffers.Get(bufferId)->GetBuffer();
        EXPECT_NE(bindGroup.GetEntries().at(1).buffer, updatedBuffer);

        bindGroup.Refresh(core);

        EXPECT_EQ(bindGroup.GetEntries().at(1).buffer, updatedBuffer);
    });

    EXPECT_NO_THROW(core.RunSystems());
}

TEST(BindGroupTest, RefreshUpdatesSamplerBindings)
{
    Engine::Core core;
    core.AddPlugins<Graphic::Plugin>();

    core.RegisterSystem<RenderingPipeline::Init>(ConfigureHeadlessGraphics, ThrowErrorIfGraphicalErrorHappened);

    core.RegisterSystem([](Engine::Core &core) {
        auto bindGroup = CreateBindGroup(core);

        auto samplerId = entt::hashed_string("bindgroup_sampler_asset");
        auto &samplers = core.GetResource<Graphic::Resource::SamplerContainer>();
        auto initialSampler = bindGroup.GetEntries().at(2).sampler;

        auto &device = core.GetResource<Graphic::Resource::Context>().deviceContext.GetDevice().value();
        samplers.Remove(samplerId);
        samplers.Add(samplerId, Graphic::Resource::Sampler(device));
        auto updatedSampler = samplers.Get(samplerId).getSampler();
        EXPECT_NE(bindGroup.GetEntries().at(2).sampler, updatedSampler);

        bindGroup.Refresh(core);

        EXPECT_EQ(bindGroup.GetEntries().at(2).sampler, updatedSampler);
    });

    EXPECT_NO_THROW(core.RunSystems());
}
