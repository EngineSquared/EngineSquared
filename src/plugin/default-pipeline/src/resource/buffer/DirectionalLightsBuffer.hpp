#pragma once

#include "component/DirectionalLight.hpp"
#include "component/GPUDirectionalLight.hpp"
#include "component/Transform.hpp"
#include "entity/Entity.hpp"
#include "exception/UpdateBufferError.hpp"
#include "resource/AGPUBuffer.hpp"
#include "resource/Context.hpp"
#include "utils/DirectionalLights.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <vector>

namespace DefaultPipeline::Resource {
class DirectionalLightsBuffer : public Graphic::Resource::AGPUBuffer {
  private:
    static inline std::string _debugName = "DirectionalLightsBuffer";

    struct GPUDirectionalLight {
        glm::mat4 viewProjectionMatrix;
        glm::vec4 color;
        glm::vec3 direction;
        uint32_t _padding;
    };

    struct GPUDirectionalLights {
        std::array<GPUDirectionalLight, Utils::MAX_DIRECTIONAL_LIGHTS> lights;
        uint32_t count;
        std::array<float, 3> _padding;
    };

  public:
    DirectionalLightsBuffer() = default;
    ~DirectionalLightsBuffer() override { Destroy(); }

    void Create(Engine::Core &core) override
    {
        const auto &context = core.GetResource<Graphic::Resource::Context>();

        _buffer = _CreateBuffer(context.deviceContext);
        _isCreated = true;

        GPUDirectionalLights data{};
        data.count = 0;
        context.queue->writeBuffer(_buffer, 0, &data, sizeof(GPUDirectionalLights));
    }

    void Destroy(Engine::Core &core) override { Destroy(); }

    void Destroy()
    {
        if (_isCreated)
        {
            _isCreated = false;
            _buffer.release();
        }
    }

    bool IsCreated(Engine::Core &core) const override { return _isCreated; }

    void Update(Engine::Core &core) override
    {
        if (!_isCreated)
        {
            throw Graphic::Exception::UpdateBufferError(
                "Cannot update a GPU directional lights buffer that is not created.");
        }

        const auto &context = core.GetResource<Graphic::Resource::Context>();
        GPUDirectionalLights data{};
        auto view = core.GetRegistry()
                        .view<Object::Component::DirectionalLight, Component::GPUDirectionalLight,
                              Object::Component::Transform>();

        uint32_t index = 0;
        uint32_t skippedCount = 0;
        view.each([&data, &index, &skippedCount](auto, const Object::Component::DirectionalLight &light,
                                                 const Component::GPUDirectionalLight &gpuLight,
                                                 const Object::Component::Transform &transform) {
            if (index >= Utils::MAX_DIRECTIONAL_LIGHTS)
            {
                skippedCount++;
                return;
            }
            const auto &viewProjectionMatrix = gpuLight.viewProjectionMatrix;
            const auto &color = light.color;
            const auto &direction = -glm::normalize(transform.GetForwardVector() * transform.GetScale());
            data.lights[index].viewProjectionMatrix = viewProjectionMatrix;
            data.lights[index].color = color;
            data.lights[index].direction = direction;
            index++;
        });
        data.count = index;

        if (skippedCount > 0)
        {
            Log::Warn(fmt::format("Maximum number of directional lights ({}) reached. {} light(s) skipped.",
                                  Utils::MAX_DIRECTIONAL_LIGHTS, skippedCount));
        }

        context.queue->writeBuffer(_buffer, 0, &data, sizeof(GPUDirectionalLights));
    }

    const wgpu::Buffer &GetBuffer() const override { return _buffer; }

    std::string_view GetDebugName() const { return _debugName; }

    static uint32_t GPUSize() { return sizeof(GPUDirectionalLights); }

  private:
    wgpu::Buffer _CreateBuffer(const Graphic::Resource::DeviceContext &context)
    {
        wgpu::BufferDescriptor bufferDesc(wgpu::Default);
        bufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform;
        bufferDesc.size = sizeof(GPUDirectionalLights);
        bufferDesc.label = wgpu::StringView(_debugName);

        return context.GetDevice()->createBuffer(bufferDesc);
    }

    wgpu::Buffer _buffer;
    bool _isCreated = false;
};
} // namespace DefaultPipeline::Resource
