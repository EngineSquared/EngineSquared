#pragma once

#include "component/AmbientLight.hpp"
#include "entity/Entity.hpp"
#include "exception/UpdateBufferError.hpp"
#include "resource/AGPUBuffer.hpp"
#include "resource/Context.hpp"
#include <glm/gtc/type_ptr.hpp>

namespace DefaultPipeline::Resource {
class AmbientLightBuffer : public Graphic::Resource::AGPUBuffer {
  private:
    static inline std::string prefix = "AmbientLightBuffer_";
    struct AmbientLightTransfer {
        glm::vec3 color;

        explicit AmbientLightTransfer(const Object::Component::AmbientLight &ambientLight) : color(ambientLight.color)
        {
        }

        static uint32_t CPUSize() { return sizeof(AmbientLightTransfer); }
        static uint32_t GPUSize() { return sizeof(AmbientLightTransfer) + sizeof(float) /*padding*/; }
    };

    static_assert(sizeof(AmbientLightTransfer) == (sizeof(float) * 3),
                  "AmbientLightTransfer struct size does not match GPU requirements.");

  public:
    explicit AmbientLightBuffer(Engine::Entity entity) : _entity(entity) { _UpdateDebugName(); }

    explicit AmbientLightBuffer(void) { _UpdateDebugName(); }

    ~AmbientLightBuffer() override { Destroy(); }
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
        if (!_entity.has_value() || _entity->IsAlive() == false)
        {
            return;
        }
        const auto &ambientLight = _entity->GetComponents<Object::Component::AmbientLight>();
        SetValue(core, ambientLight);
    };

    void SetEntity(Engine::Entity entity)
    {
        _entity = entity;
        _UpdateDebugName();
    }

    void SetValue(const Engine::Core &core, const Object::Component::AmbientLight &ambientLight)
    {
        if (!_isCreated)
        {
            throw Graphic::Exception::UpdateBufferError(
                "Cannot update a GPU ambient light buffer that is not created.");
        }

        const auto &context = core.GetResource<Graphic::Resource::Context>();
        _UpdateBuffer(context, ambientLight);
    }

    const wgpu::Buffer &GetBuffer() const override { return _buffer; };

    std::string_view GetDebugName() const { return _debugName; }

  private:
    inline void _UpdateDebugName()
    {
        if (_entity.has_value())
        {
            _debugName = fmt::format("{}{}", prefix, *_entity);
        }
        else
        {
            _debugName = fmt::format("{}{}", prefix, "<no_entity>");
        }
    }

    wgpu::Buffer _CreateBuffer(const Graphic::Resource::DeviceContext &context)
    {
        wgpu::BufferDescriptor bufferDesc(wgpu::Default);
        bufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform;
        bufferDesc.size = AmbientLightTransfer::GPUSize();
        bufferDesc.label = wgpu::StringView(_debugName);

        return context.GetDevice()->createBuffer(bufferDesc);
    }

    void _UpdateBuffer(const Graphic::Resource::Context &context,
                       const Object::Component::AmbientLight &ambientLightComponent)
    {
        AmbientLightTransfer ambientLightTransfer(ambientLightComponent);
        context.queue->writeBuffer(_buffer, 0, std::addressof(ambientLightTransfer), AmbientLightTransfer::CPUSize());
    }

    wgpu::Buffer _buffer;
    bool _isCreated = false;
    std::optional<Engine::Entity> _entity;
    std::string _debugName;
};
} // namespace DefaultPipeline::Resource
