#include <gtest/gtest.h>

#include "Engine.hpp"
#include "Graphic.hpp"
#include "RenderingPipeline.hpp"

class GPUBufferTest : public Graphic::Resource::AGPUBuffer {
  public:
    explicit GPUBufferTest(std::array<int, 5> &&data) : _data(std::move(data)) {}

    void Create(Engine::Core &core) override
    {
        const auto &context = core.GetResource<Graphic::Resource::Context>();
        const auto &optDevice = context.deviceContext.GetDevice();
        if (!optDevice.has_value())
        {
            Log::Error("GPUBufferTest: context.deviceContext.GetDevice has no value");
            return;
        }

        wgpu::BufferDescriptor bufferDesc(wgpu::Default);
        std::string label = "GPUBufferTest";
        bufferDesc.label = wgpu::StringView(label);
        bufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform;
        bufferDesc.size = sizeof(int) * _data.size();

        _buffer = optDevice.value().createBuffer(bufferDesc);
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
    void Update(Engine::Core &core) override
    {
        if (!_isCreated)
        {
            throw Graphic::Exception::UpdateBufferError("Cannot update a GPU buffer that is not created.");
        }
        if (!core.HasResource<Graphic::Resource::Queue>())
        {
            Log::Error("GPUBufferTest::Update: context.queue has no value");
            return;
        }
        const auto &queue = core.GetResource<Graphic::Resource::Queue>();
        queue->writeBuffer(_buffer, 0, _data.data(), sizeof(int) * _data.size());
    }
    const wgpu::Buffer &GetBuffer() const override { return _buffer; }

  private:
    bool _isCreated = false;
    wgpu::Buffer _buffer;
    std::array<int, 5> _data;
};

void TestSystem(Engine::Core &core)
{
    std::array<int, 5> data = {1, 2, 3, 4, 5};

    auto buffer = GPUBufferTest(std::move(data));

    EXPECT_FALSE(buffer.IsCreated(core));
    buffer.Create(core);
    EXPECT_TRUE(buffer.IsCreated(core));

    EXPECT_NO_THROW(buffer.Update(core));

    EXPECT_EQ(buffer.GetBuffer().getSize(), sizeof(int) * 5);

    buffer.Destroy(core);
    EXPECT_FALSE(buffer.IsCreated(core));
}

TEST(GraphicPlugin, GlobalRun)
{
    Engine::Core core;

    core.AddPlugins<Graphic::Plugin>();

    core.RegisterSystem<RenderingPipeline::Init>([](Engine::Core &c) {
        c.GetResource<Graphic::Resource::GraphicSettings>().SetWindowSystem(Graphic::Resource::WindowSystem::None);
    });

    core.RegisterSystem(TestSystem);

    EXPECT_NO_THROW(core.RunSystems());
}
