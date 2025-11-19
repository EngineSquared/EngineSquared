#include <gtest/gtest.h>

#include "Graphic.hpp"
#include "RenderingPipeline.hpp"

#include <glm/glm.hpp>

namespace {

const std::string bindGroupShaderSource = R"(
@group(0) @binding(0) var testTexture: texture_2d<f32>;

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
    return textureLoad(testTexture, vec2i(0, 0), 0);
}
)";

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
                                             .setBinding(0));

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

} // namespace

TEST(BindGroupTest, CreatesEntriesForTextureAssets)
{
    Engine::Core core;
    core.AddPlugins<Graphic::Plugin>();

    core.RegisterSystem<RenderingPipeline::Init>(ConfigureHeadlessGraphics);

    core.RegisterSystem([](Engine::Core &core) {
        auto shaderId = entt::hashed_string("bindgroup_texture_shader");
        auto textureId = entt::hashed_string("bindgroup_texture_asset");

        core.GetResource<Graphic::Resource::ShaderContainer>().Add(shaderId, CreateShader(core));

        auto &context = core.GetResource<Graphic::Resource::Context>();
        auto &textures = core.GetResource<Graphic::Resource::TextureContainer>();
        auto image = Graphic::Resource::Image(glm::uvec2(2, 2), [](glm::uvec2) { return glm::u8vec4(255, 0, 0, 255); });
        textures.Add(textureId, context, "BindGroupTextureA", image);

        Graphic::Resource::BindGroup bindGroup(
            core, shaderId, 0,
            {
                {0, Graphic::Resource::BindGroup::Asset::Type::Texture, textureId, 0}
        });

        const auto &entries = bindGroup.GetEntries();
        ASSERT_EQ(entries.size(), 1u);
        EXPECT_EQ(entries.front().binding, 0u);
        EXPECT_EQ(entries.front().textureView, textures.Get(textureId).GetDefaultView());
        EXPECT_TRUE(bindGroup.GetBindGroup());
    });

    EXPECT_NO_THROW(core.RunSystems());
}

TEST(BindGroupTest, RefreshUpdatesTextureBindings)
{
    Engine::Core core;
    core.AddPlugins<Graphic::Plugin>();

    core.RegisterSystem<RenderingPipeline::Init>(ConfigureHeadlessGraphics);

    core.RegisterSystem([](Engine::Core &core) {
        auto shaderId = entt::hashed_string("bindgroup_refresh_shader");
        auto textureId = entt::hashed_string("bindgroup_refresh_texture");

        core.GetResource<Graphic::Resource::ShaderContainer>().Add(shaderId, CreateShader(core));

        auto &context = core.GetResource<Graphic::Resource::Context>();
        auto &textures = core.GetResource<Graphic::Resource::TextureContainer>();
        textures.Add(
            textureId, context, "BindGroupTextureA",
            Graphic::Resource::Image(glm::uvec2(2, 2), [](glm::uvec2) { return glm::u8vec4(10, 20, 30, 255); }));

        Graphic::Resource::BindGroup bindGroup(
            core, shaderId, 0,
            {
                {0, Graphic::Resource::BindGroup::Asset::Type::Texture, textureId, 0}
        });

        auto initialView = bindGroup.GetEntries().front().textureView;

        textures.Add(
            textureId, context, "BindGroupTextureB",
            Graphic::Resource::Image(glm::uvec2(2, 2), [](glm::uvec2) { return glm::u8vec4(200, 100, 50, 255); }));
        auto updatedView = textures.Get(textureId).GetDefaultView();
        EXPECT_NE(initialView, updatedView);

        bindGroup.Refresh(core);

        EXPECT_EQ(bindGroup.GetEntries().front().textureView, updatedView);
    });

    EXPECT_NO_THROW(core.RunSystems());
}
