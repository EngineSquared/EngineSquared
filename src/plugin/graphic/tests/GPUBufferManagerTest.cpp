#include <gtest/gtest.h>

#include <entt/core/hashed_string.hpp>

#include <memory>
#include <string>

#include "Graphic.hpp"
#include "RenderingPipeline.hpp"

namespace {
class DummyGPUBuffer final : public Graphic::Resource::AGPUBuffer {
  public:
    explicit DummyGPUBuffer(std::string label) : _label(std::move(label)) {}

    void Create(Engine::Core &) override {}
    void Destroy(Engine::Core &) override {}
    bool IsCreated(Engine::Core &) const override { return true; }
    void Update(Engine::Core &) override {}
    const wgpu::Buffer &GetBuffer() const override { return _buffer; }

    const std::string &Label() const { return _label; }

  private:
    std::string _label;
    wgpu::Buffer _buffer;
};

auto TestGPUBufferContainerSystem(Engine::Core &core) -> void
{
    auto &gpuBufferManager = core.GetResource<Graphic::Resource::GPUBufferContainer>();

    const entt::hashed_string buffer1Id{"test_buffer_1"};
    const entt::hashed_string buffer2Id{"test_buffer_2"};

    gpuBufferManager.Add(buffer1Id, std::make_unique<DummyGPUBuffer>("BufferOne"));
    gpuBufferManager.Add(buffer2Id, std::make_unique<DummyGPUBuffer>("BufferTwo"));

    EXPECT_TRUE(gpuBufferManager.Contains(buffer1Id));
    EXPECT_TRUE(gpuBufferManager.Contains(buffer2Id));

    auto &storedBuffer1 = gpuBufferManager.Get(buffer1Id);
    auto &storedBuffer2 = gpuBufferManager.Get(buffer2Id);

    auto *dummyBuffer1 = dynamic_cast<DummyGPUBuffer *>(storedBuffer1.get());
    auto *dummyBuffer2 = dynamic_cast<DummyGPUBuffer *>(storedBuffer2.get());

    ASSERT_NE(dummyBuffer1, nullptr);
    ASSERT_NE(dummyBuffer2, nullptr);

    EXPECT_EQ(dummyBuffer1->Label(), "BufferOne");
    EXPECT_EQ(dummyBuffer2->Label(), "BufferTwo");

    gpuBufferManager.Remove(buffer1Id);

    EXPECT_FALSE(gpuBufferManager.Contains(buffer1Id));
    EXPECT_TRUE(gpuBufferManager.Contains(buffer2Id));
    EXPECT_THROW((void) gpuBufferManager.Get(buffer1Id), Object::ResourceManagerError);
}
} // namespace

TEST(GPUBufferManagerTest, BasicOperations)
{
    Engine::Core core;

    [[maybe_unused]] auto &containerResource =
        core.RegisterResource<Graphic::Resource::GPUBufferContainer>(Graphic::Resource::GPUBufferContainer());

    core.RegisterSystem(TestGPUBufferContainerSystem);

    EXPECT_NO_THROW(core.RunSystems());
}
