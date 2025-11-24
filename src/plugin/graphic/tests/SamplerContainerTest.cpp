#include <gtest/gtest.h>

#include "Graphic.hpp"
#include "RenderingPipeline.hpp"
#include "resource/SamplerContainer.hpp"

Graphic::Resource::Sampler CreateTestSampler(Engine::Core &core)
{
    auto &device = core.GetResource<Graphic::Resource::Context>().deviceContext.GetDevice();

    if (!device.has_value()) {
        throw Object::ResourceManagerError("Device is not available in the context");
    }

    return Graphic::Resource::Sampler(device.value());
}

Graphic::Resource::Sampler CreateTestSamplerWithDescriptor(Engine::Core &core)
{
    auto &device = core.GetResource<Graphic::Resource::Context>().deviceContext.GetDevice();

    if (!device.has_value()) {
        throw Object::ResourceManagerError("Device is not available in the context");
    }
    
    wgpu::SamplerDescriptor samplerDesc(wgpu::Default);
    samplerDesc.addressModeU = wgpu::AddressMode::Repeat;
    samplerDesc.addressModeV = wgpu::AddressMode::Repeat;
    samplerDesc.addressModeW = wgpu::AddressMode::Repeat;
    samplerDesc.magFilter = wgpu::FilterMode::Nearest;
    samplerDesc.minFilter = wgpu::FilterMode::Nearest;
    samplerDesc.mipmapFilter = wgpu::MipmapFilterMode::Nearest;
    samplerDesc.lodMinClamp = 0.0f;
    samplerDesc.lodMaxClamp = 1.0f;
    samplerDesc.compare = wgpu::CompareFunction::Undefined;
    samplerDesc.maxAnisotropy = 1;
    
    return Graphic::Resource::Sampler(device.value(), samplerDesc);
}

auto TestSamplerContainerSystem(Engine::Core &core) -> void
{
    Graphic::Resource::SamplerContainer samplerContainer;

    entt::hashed_string sampler1Id = "test_sampler_1";
    entt::hashed_string sampler2Id = "test_sampler_2";

    auto sampler1 = CreateTestSampler(core);
    auto sampler2 = CreateTestSamplerWithDescriptor(core);

    samplerContainer.Add(sampler1Id, std::move(sampler1));
    samplerContainer.Add(sampler2Id, std::move(sampler2));

    EXPECT_TRUE(samplerContainer.Contains(sampler1Id));
    EXPECT_TRUE(samplerContainer.Contains(sampler2Id));

    samplerContainer.Remove(sampler1Id);
    EXPECT_FALSE(samplerContainer.Contains(sampler1Id));
    EXPECT_TRUE(samplerContainer.Contains(sampler2Id));

    EXPECT_THROW(
        [&]() {
            auto &unused = samplerContainer.Get(sampler1Id);
            (void)unused;
        }(),
        Object::ResourceManagerError
    );
}

TEST(SamplerContainerTest, GlobalRun)
{
    Engine::Core core;

    core.AddPlugins<Graphic::Plugin>();

    core.RegisterSystem<RenderingPipeline::Init>([](Engine::Core &c) {
        c.GetResource<Graphic::Resource::GraphicSettings>()
            .SetWindowSystem(Graphic::Resource::WindowSystem::None)
            .SetOnErrorCallback([](WGPUDevice const *, WGPUErrorType type, WGPUStringView message,
                                   WGPU_NULLABLE void *, WGPU_NULLABLE void *) {
                Log::Error(fmt::format("Custom uncaptured device error: type {:x} ({})", static_cast<uint32_t>(type),
                                       std::string(message.data, message.length)));
                throw Graphic::Exception::UncapturedDeviceError("Custom uncaptured device error occurred");
            })
            .GetWantedLimits()
            .maxBindGroups = 8;
    });

    core.RegisterSystem(TestSamplerContainerSystem);

    core.RunSystems();
}