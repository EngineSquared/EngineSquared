#pragma once

#include "component/Mesh.hpp"
#include "entity/Entity.hpp"
#include "exception/NonexistentComponentError.hpp"
#include "exception/UpdateBufferError.hpp"
#include "resource/AGPUBuffer.hpp"
#include "resource/Context.hpp"

namespace DefaultPipeline::Resource {
class IndexGPUBuffer : public Graphic::Resource::AGPUBuffer {
  public:
    explicit IndexGPUBuffer(Engine::Entity entity) : _entity(entity) {}
    ~IndexGPUBuffer() override = default;
    void Create(Engine::Core &core) override
    {

        auto meshComponent = _entity.TryGetComponent<Object::Component::Mesh>(core);

        if (!meshComponent)
        {
            throw Graphic::Exception::NonexistentComponentError(
                "Cannot create a GPU buffer for an entity without a Mesh component.");
        }

        const auto &indices = meshComponent->indices;

        wgpu::BufferDescriptor bufferDesc(wgpu::Default);
        bufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Index;
        bufferDesc.size = sizeof(uint32_t) * indices.size();
        std::string label =
            "IndexGPUBuffer_" + Log::EntityToDebugString(static_cast<Engine::Entity::entity_id_type>(_entity));
        bufferDesc.label = wgpu::StringView(label);

        const auto &context = core.GetResource<Graphic::Resource::Context>();

        _buffer = context.deviceContext.GetDevice()->createBuffer(bufferDesc);

        context.queue->writeBuffer(_buffer, 0, indices.data(), bufferDesc.size);

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
            throw Graphic::Exception::UpdateBufferError("Cannot update a GPU buffer that is not created.");
        }

        auto &meshComponent = core.GetRegistry().get<Object::Component::Mesh>(_entity);

        // For now, we will not implement dynamic resizing of the buffer. As we should have a way to know if the size
        // changed. And it would be so heavy to check every frame every index.
    };

    const wgpu::Buffer &GetBuffer() const override { return _buffer; };

  private:
    wgpu::Buffer _buffer;
    bool _isCreated = false;
    Engine::Entity _entity = Engine::Entity::entity_null_id;
};
} // namespace DefaultPipeline::Resource
