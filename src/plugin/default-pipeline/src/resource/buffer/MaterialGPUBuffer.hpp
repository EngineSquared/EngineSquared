#pragma once

#include "component/Material.hpp"
#include "entity/Entity.hpp"
#include "exception/UpdateBufferError.hpp"
#include "resource/AGPUBuffer.hpp"
#include <glm/gtc/type_ptr.hpp>

namespace DefaultPipeline::Resource {
class MaterialGPUBuffer : public Graphic::Resource::AGPUBuffer {
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
    explicit MaterialGPUBuffer(Engine::Entity entity) : _entity(entity) { _UpdateDebugName(); }

    explicit MaterialGPUBuffer(void) = default;

    ~MaterialGPUBuffer() override = default;
    void Create(Engine::Core &core) override
    {
        const auto &context = core.GetResource<Graphic::Resource::Context>();

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
        if (!_isCreated)
        {
            throw Graphic::Exception::UpdateBufferError("Cannot update a GPU material buffer that is not created.");
        }

        if (!_entity.IsValid())
        {
            return;
        }

        const auto &materialComponent = _entity.GetComponents<Object::Component::Material>();
        const auto &context = core.GetResource<Graphic::Resource::Context>();
        _UpdateBuffer(materialComponent, context);
    };

    void SetMaterial(const Engine::Core &core, const Object::Component::Material &material)
    {
        if (!_isCreated)
        {
            throw Graphic::Exception::UpdateBufferError("Cannot update a GPU material buffer that is not created.");
        }

        const auto &context = core.GetResource<Graphic::Resource::Context>();
        _UpdateBuffer(material, context);
    }

    const wgpu::Buffer &GetBuffer() const override { return _buffer; };

    std::string_view GetDebugName() const { return _debugName; }

  private:
    void _UpdateDebugName() { _debugName = fmt::format("{}{}", prefix, _entity); }

    wgpu::Buffer _CreateBuffer(const Graphic::Resource::DeviceContext &context)
    {
        wgpu::BufferDescriptor bufferDesc(wgpu::Default);
        bufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform;
        bufferDesc.size = MaterialTransfer::GPUSize();
        bufferDesc.label = wgpu::StringView(_debugName);

        return context.GetDevice()->createBuffer(bufferDesc);
    }

    void _UpdateBuffer(const Object::Component::Material &materialComponent, const Graphic::Resource::Context &context)
    {
        MaterialTransfer materialTransfer(materialComponent);
        context.queue->writeBuffer(_buffer, 0, std::addressof(materialTransfer), MaterialTransfer::CPUSize());
    }

    wgpu::Buffer _buffer;
    bool _isCreated = false;
    Engine::Entity _entity = Engine::Entity::Null();
    std::string _debugName;
};
} // namespace DefaultPipeline::Resource
