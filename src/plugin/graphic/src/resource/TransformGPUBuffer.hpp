#pragma once

#include "component/Mesh.hpp"
#include "entity/Entity.hpp"
#include "exception/NonexistentComponentError.hpp"
#include "exception/UpdateBufferError.hpp"
#include "resource/AGPUBuffer.hpp"
#include "component/Transform.hpp"
#include <glm/gtc/type_ptr.hpp>

namespace Graphic::Resource {
class TransformGPUBuffer : public AGPUBuffer {
  public:
    explicit TransformGPUBuffer(Engine::Entity entity) : _entity(entity) {}
    ~TransformGPUBuffer() override = default;
    void Create(Engine::Core &core) override
    {
        const Object::Component::Transform &transformComponent = _entity.GetComponents<Object::Component::Transform>(core);

        wgpu::BufferDescriptor bufferDesc(wgpu::Default);
        bufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform;
        bufferDesc.size = sizeof(glm::mat4);
        std::string label = "TransformGPUBuffer_" + Log::EntityToDebugString(static_cast<Engine::Entity::entity_id_type>(_entity));
        bufferDesc.label = wgpu::StringView(label);

        _buffer = core.GetResource<Context>().deviceContext.GetDevice()->createBuffer(bufferDesc);

        glm::mat4 modelMatrix = transformComponent.GetTransformationMatrix();
        core.GetResource<Context>().queue->writeBuffer(_buffer, 0, glm::value_ptr(modelMatrix), bufferDesc.size);

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

        // TODO: implement update logic
    };

    const wgpu::Buffer &GetBuffer() const override { return _buffer; };

  private:
    wgpu::Buffer _buffer;
    bool _isCreated = false;
    Engine::Entity _entity = Engine::Entity::entity_null_id;
};
} // namespace Graphic::Resource
