#pragma once

#include "component/Mesh.hpp"
#include "component/Transform.hpp"
#include "entity/Entity.hpp"
#include "exception/NonexistentComponentError.hpp"
#include "exception/UpdateBufferError.hpp"
#include "resource/AGPUBuffer.hpp"
#include <glm/gtc/type_ptr.hpp>

namespace Graphic::Resource {
class TransformGPUBuffer : public AGPUBuffer {
  public:
    explicit TransformGPUBuffer(Engine::Entity entity) : _entity(entity) {}
    ~TransformGPUBuffer() override = default;
    void Create(Engine::Core &core) override
    {
        Object::Component::Transform &transformComponent =
            _entity.GetComponents<Object::Component::Transform>(core);
        const Context &context = core.GetResource<Context>();

        _buffer = _CreateBuffer(context.deviceContext);
        _UpdateBuffer(transformComponent, context);
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
            throw Exception::UpdateBufferError("Cannot update a GPU buffer that is not created.");
        }

        Object::Component::Transform &transformComponent =
            _entity.GetComponents<Object::Component::Transform>(core);
        const Context &context = core.GetResource<Context>();
        _UpdateBuffer(transformComponent, context);
    };

    const wgpu::Buffer &GetBuffer() const override { return _buffer; };

  private:
    wgpu::Buffer _CreateBuffer(const DeviceContext &context)
    {
        wgpu::BufferDescriptor bufferDesc(wgpu::Default);
        bufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform;
        bufferDesc.size = sizeof(glm::mat4);
        std::string label =
            "TransformGPUBuffer_" + Log::EntityToDebugString(static_cast<Engine::Entity::entity_id_type>(_entity));
        bufferDesc.label = wgpu::StringView(label);

        return context.GetDevice()->createBuffer(bufferDesc);
    }

    void _UpdateBuffer(Object::Component::Transform &transformComponent, const Context &context)
    {
        glm::mat4 modelMatrix = transformComponent.ComputeTransformationMatrix();
        context.queue->writeBuffer(_buffer, 0, glm::value_ptr(modelMatrix), sizeof(glm::mat4));
    }

    wgpu::Buffer _buffer;
    bool _isCreated = false;
    Engine::Entity _entity = Engine::Entity::entity_null_id;
};
} // namespace Graphic::Resource
