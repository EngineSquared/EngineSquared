#pragma once

#include "component/PointLight.hpp"
#include "component/Transform.hpp"
#include "entity/Entity.hpp"
#include "exception/UpdateBufferError.hpp"
#include "resource/AGPUBuffer.hpp"
#include "resource/Context.hpp"
#include "utils/PointLight.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <vector>

namespace Graphic::Resource {
class PointLightsBuffer : public AGPUBuffer {
  private:
    static inline std::string _debugName = "PointLightsBuffer";

    struct GPUPointLight {
        float position[3];
        float intensity;
        float color[3];
        float radius;
        float falloff;
        float _padding[3];
    };

    static_assert(sizeof(GPUPointLight) == 48, "GPUPointLight must be 48 bytes for proper GPU alignment.");

    struct PointLightsData {
        GPUPointLight lights[Utils::MAX_POINT_LIGHTS]; // 64 * 48 = 3072 bytes
        uint32_t count;                                // 4 bytes (3076 bytes)
        float _padding[3];                             // 12 bytes (3088 bytes)
    };

    static_assert(sizeof(PointLightsData) == (48 * Utils::MAX_POINT_LIGHTS + 16),
                  "PointLightsData size does not match expected GPU requirements.");

  public:
    PointLightsBuffer() = default;
    ~PointLightsBuffer() override = default;

    void Create(Engine::Core &core) override
    {
        const Context &context = core.GetResource<Context>();

        wgpu::BufferDescriptor bufferDesc(wgpu::Default);
        bufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform;
        bufferDesc.size = sizeof(PointLightsData);
        bufferDesc.label = wgpu::StringView(_debugName);

        _buffer = context.deviceContext.GetDevice()->createBuffer(bufferDesc);
        _isCreated = true;

        PointLightsData data{};
        data.count = 0;
        context.queue->writeBuffer(_buffer, 0, &data, sizeof(PointLightsData));
    }

    void Destroy(Engine::Core &core) override
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
            throw Exception::UpdateBufferError("Cannot update a GPU point lights buffer that is not created.");
        }

        const Context &context = core.GetResource<Context>();
        PointLightsData data{};

        auto view = core.GetRegistry().view<Object::Component::PointLight, Object::Component::Transform>();

        uint32_t index = 0;
        view.each([&](const Engine::Entity &entity, const Object::Component::PointLight &light, const Object::Component::Transform &transform)
        {
            if (index >= Utils::MAX_POINT_LIGHTS)
                return;
            const glm::vec3 &pos = transform.GetPosition();
            const glm::vec3 &col = light.color;

            data.lights[index].position[0] = pos.x;
            data.lights[index].position[1] = pos.y;
            data.lights[index].position[2] = pos.z;
            data.lights[index].intensity = light.intensity;
            data.lights[index].color[0] = col.x;
            data.lights[index].color[1] = col.y;
            data.lights[index].color[2] = col.z;
            data.lights[index].radius = light.radius;
            data.lights[index].falloff = light.falloff;
            data.lights[index]._padding[0] = 0.0f;
            data.lights[index]._padding[1] = 0.0f;
            data.lights[index]._padding[2] = 0.0f;
            ++index;
        });
        data.count = index;

        context.queue->writeBuffer(_buffer, 0, &data, sizeof(PointLightsData));
    }

    const wgpu::Buffer &GetBuffer() const override { return _buffer; }

    std::string_view GetDebugName() const { return _debugName; }

    static uint32_t GPUSize() { return sizeof(PointLightsData); }

  private:
    wgpu::Buffer _buffer;
    bool _isCreated = false;
};
} // namespace Graphic::Resource
