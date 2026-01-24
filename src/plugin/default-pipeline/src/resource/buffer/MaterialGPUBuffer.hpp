#pragma once

#include "component/Material.hpp"
#include "entity/Entity.hpp"
#include "exception/UpdateBufferError.hpp"
#include "resource/AGPUBuffer.hpp"
#include <glm/gtc/type_ptr.hpp>

namespace DefaultPipeline::Resource {
class MaterialGPUBuffer : public Graphic::Resource::AGPUBuffer {
  public:
    static inline std::string prefix = "MaterialGPUBuffer_";
    struct MaterialTransfer {
        glm::vec4 ambient;
        glm::vec4 diffuse;
        glm::vec4 specular;
        glm::vec4 transmittance;
        glm::vec4 emission;
        float _padding[3];
        float shininess;

        explicit MaterialTransfer(const Object::Component::Material &material)
            : ambient(material.ambient, 1.0f), diffuse(material.diffuse, 1.0f), specular(material.specular, 1.0f),
              transmittance(material.transmittance, 1.0f), emission(material.emission, 1.0f),
              shininess(material.shininess)
        {
        }

        static uint32_t CPUSize() { return sizeof(MaterialTransfer); }
        static uint32_t GPUSize() { return sizeof(MaterialTransfer); }
    };

    explicit MaterialGPUBuffer(Engine::Entity entity) : _entity(entity) { _UpdateDebugName(); }

    explicit MaterialGPUBuffer(void) { _UpdateDebugName(); }

    ~MaterialGPUBuffer() override { Destroy(); }
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
        if (!_isCreated)
        {
            throw Graphic::Exception::UpdateBufferError("Cannot update a GPU material buffer that is not created.");
        }

        if (!_entity.has_value())
        {
            return;
        }

        const auto &materialComponent = _entity->GetComponents<Object::Component::Material>();
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
    void _UpdateDebugName()
    {
        if (_entity.has_value())
            _debugName = fmt::format("{}{}", prefix, *_entity);
        else
            _debugName = fmt::format("{}{}", prefix, "Default");
    }

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
    std::optional<Engine::Entity> _entity;
    std::string _debugName;
};
} // namespace DefaultPipeline::Resource
