#pragma once

#include "component/AmbientLight.hpp"
#include "entity/Entity.hpp"
#include "exception/UpdateBufferError.hpp"
#include "resource/AGPUBuffer.hpp"
#include "resource/Context.hpp"
#include <glm/gtc/type_ptr.hpp>

namespace Graphic::Resource {
class AmbientLightBuffer : public AGPUBuffer {
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
    explicit AmbientLightBuffer(Engine::Entity entity = Engine::Entity{}) : _entity(entity)
    {
        std::string entityName = "NULL_ENTITY";
        if (_entity.IsValid())
        {
            entityName = Log::EntityToDebugString(static_cast<Engine::Entity::entity_id_type>(entity));
        }
        _debugName = fmt::format("{}{}", prefix, "NULL_ENTITY");
    }

    ~AmbientLightBuffer() override = default;
    void Create(Engine::Core &core) override
    {
        const Context &context = core.GetResource<Context>();

        _buffer = _CreateBuffer(context.deviceContext);
        _isCreated = true;
    };
    void Destroy(Engine::Core &core) override
    {
        if (_isCreated)
        {
            _isCreated = false;
            _buffer.release();
        }
    };

    bool IsCreated(Engine::Core &core) const override { return _isCreated; };
    void Update(Engine::Core &core) override
    {
        if (_entity == Engine::Entity::entity_null_id)
        {
            return;
        }
        const auto &ambientLight = _entity.GetComponents<Object::Component::AmbientLight>(core);
        SetValue(core, ambientLight);
    };

    void SetEntity(Engine::Entity entity) { _entity = entity; }

    void SetValue(const Engine::Core &core, const Object::Component::AmbientLight &ambientLight)
    {
        if (!_isCreated)
        {
            throw Exception::UpdateBufferError("Cannot update a GPU ambient light buffer that is not created.");
        }

        const Context &context = core.GetResource<Context>();
        _UpdateBuffer(context, ambientLight);
    }

    const wgpu::Buffer &GetBuffer() const override { return _buffer; };

    std::string_view GetDebugName() const { return _debugName; }

  private:
    wgpu::Buffer _CreateBuffer(const DeviceContext &context)
    {
        wgpu::BufferDescriptor bufferDesc(wgpu::Default);
        bufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform;
        bufferDesc.size = AmbientLightTransfer::GPUSize();
        bufferDesc.label = wgpu::StringView(_debugName);

        return context.GetDevice()->createBuffer(bufferDesc);
    }

    void _UpdateBuffer(const Context &context, const Object::Component::AmbientLight &ambientLightComponent)
    {
        AmbientLightTransfer ambientLightTransfer(ambientLightComponent);
        context.queue->writeBuffer(_buffer, 0, std::addressof(ambientLightTransfer), AmbientLightTransfer::CPUSize());
    }

    wgpu::Buffer _buffer;
    bool _isCreated = false;
    Engine::Entity _entity = Engine::Entity::entity_null_id;
    std::string _debugName;
};
} // namespace Graphic::Resource
