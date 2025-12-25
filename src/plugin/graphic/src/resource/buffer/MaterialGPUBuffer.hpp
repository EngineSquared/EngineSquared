#pragma once

#include "component/Material.hpp"
#include "entity/Entity.hpp"
#include "exception/UpdateBufferError.hpp"
#include "resource/AGPUBuffer.hpp"
#include <glm/gtc/type_ptr.hpp>

namespace Graphic::Resource {
class MaterialGPUBuffer : public AGPUBuffer {
  private:
    static inline std::string prefix = "MaterialGPUBuffer_";
    struct MaterialTransfer {
        glm::vec3 ambient;

        explicit MaterialTransfer(const Object::Component::Material &material) : ambient(material.ambient) {}

        static uint32_t CPUSize() { return sizeof(MaterialTransfer); }
        static uint32_t GPUSize() { return sizeof(MaterialTransfer) + sizeof(float) /*padding*/; }
    };

    static_assert(sizeof(MaterialTransfer) == (sizeof(float) * 3),
                  "MaterialTransfer struct size does not match GPU requirements.");

  public:
    explicit MaterialGPUBuffer(Engine::Entity entity) : _entity(entity)
    {
        _debugName = prefix + Log::EntityToDebugString(static_cast<Engine::Entity::entity_id_type>(entity));
    }
    ~MaterialGPUBuffer() override = default;
    void Create(Engine::Core &core) override
    {
        const auto &materialComponent = _entity.GetComponents<Object::Component::Material>(core);
        const Context &context = core.GetResource<Context>();

        _buffer = _CreateBuffer(context.deviceContext);
        _UpdateBuffer(materialComponent, context);
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
        if (!_isCreated)
        {
            throw Exception::UpdateBufferError("Cannot update a GPU material buffer that is not created.");
        }

        const auto &materialComponent = _entity.GetComponents<Object::Component::Material>(core);
        const Context &context = core.GetResource<Context>();
        _UpdateBuffer(materialComponent, context);
    };

    const wgpu::Buffer &GetBuffer() const override { return _buffer; };

    std::string_view GetDebugName() const { return _debugName; }

  private:
    wgpu::Buffer _CreateBuffer(const DeviceContext &context)
    {
        wgpu::BufferDescriptor bufferDesc(wgpu::Default);
        bufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform;
        bufferDesc.size = MaterialTransfer::GPUSize();
        bufferDesc.label = wgpu::StringView(_debugName);

        return context.GetDevice()->createBuffer(bufferDesc);
    }

    void _UpdateBuffer(const Object::Component::Material &materialComponent, const Context &context)
    {
        MaterialTransfer materialTransfer(materialComponent);
        context.queue->writeBuffer(_buffer, 0, std::addressof(materialTransfer), MaterialTransfer::CPUSize());
    }

    wgpu::Buffer _buffer;
    bool _isCreated = false;
    Engine::Entity _entity = Engine::Entity::entity_null_id;
    std::string _debugName;
};
} // namespace Graphic::Resource
