#pragma once

#include "component/AmbientLight.hpp"
#include "component/GPUDirectionalLight.hpp"
#include "entity/Entity.hpp"
#include "exception/UpdateBufferError.hpp"
#include "resource/AGPUBuffer.hpp"
#include "resource/Context.hpp"
#include <glm/gtc/type_ptr.hpp>

namespace DefaultPipeline::Resource {
class DirectionalLightBuffer : public Graphic::Resource::AGPUBuffer {
  private:
    static inline std::string prefix = "DirectionalLightBuffer_";
    struct DirectionalLightTransfer {
        glm::mat4 viewProjectionMatrix;
        glm::vec4 color;
        glm::vec3 direction;

        explicit DirectionalLightTransfer(const Component::GPUDirectionalLight &GPUDirectionalLight,
                                          const Object::Component::DirectionalLight &directionalLight)
            : viewProjectionMatrix(GPUDirectionalLight.viewProjectionMatrix), color(directionalLight.color),
              direction(directionalLight.direction)
        {
        }

        static uint32_t CPUSize() { return sizeof(DirectionalLightTransfer); }
        static uint32_t GPUSize() { return sizeof(DirectionalLightTransfer) + sizeof(float) /*padding*/; }
    };

  public:
    explicit DirectionalLightBuffer(Engine::Entity entity) : _entity(entity) { _UpdateDebugName(); }

    ~DirectionalLightBuffer() override { Destroy(); }

    void Create(Engine::Core &core) override
    {
        const auto &context = core.GetResource<Graphic::Resource::Context>();

        _buffer = _CreateBuffer(context.deviceContext);
        _isCreated = true;
    };
    void Destroy(Engine::Core &core) override { Destroy(); };

    void Destroy()
    {
        if (_isCreated)
        {
            _isCreated = false;
            _buffer.release();
        }
    }

    bool IsCreated(Engine::Core &core) const override { return _isCreated; };
    void Update(Engine::Core &core) override
    {
        if (_entity.IsAlive() == false)
        {
            return;
        }
        const auto &gpuDirectionalLight = _entity.GetComponents<Component::GPUDirectionalLight>();
        const auto &directionalLight = _entity.GetComponents<Object::Component::DirectionalLight>();
        const auto &context = core.GetResource<Graphic::Resource::Context>();
        _UpdateBuffer(context, gpuDirectionalLight, directionalLight);
    };

    const wgpu::Buffer &GetBuffer() const override { return _buffer; };

    std::string_view GetDebugName() const { return _debugName; }

  private:
    inline void _UpdateDebugName()
    {
        _debugName = fmt::format("{}{}", prefix, _entity);
    }

    wgpu::Buffer _CreateBuffer(const Graphic::Resource::DeviceContext &context)
    {
        wgpu::BufferDescriptor bufferDesc(wgpu::Default);
        bufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform;
        bufferDesc.size = DirectionalLightTransfer::GPUSize();
        bufferDesc.label = wgpu::StringView(_debugName);

        return context.GetDevice()->createBuffer(bufferDesc);
    }

    void _UpdateBuffer(const Graphic::Resource::Context &context,
                       const Component::GPUDirectionalLight &GPUDirectionalLight,
                       const Object::Component::DirectionalLight &directionalLightComponent)
    {
        DirectionalLightTransfer directionalLightTransfer(GPUDirectionalLight, directionalLightComponent);
        context.queue->writeBuffer(_buffer, 0, std::addressof(directionalLightTransfer),
                                   DirectionalLightTransfer::CPUSize());
    }

    wgpu::Buffer _buffer;
    bool _isCreated = false;
    Engine::Entity _entity;
    std::string _debugName;
};
} // namespace DefaultPipeline::Resource
