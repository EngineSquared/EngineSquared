#pragma once

#include "component/Mesh.hpp"
#include "entity/Entity.hpp"
#include "exception/NonexistentComponentError.hpp"
#include "exception/UpdateBufferError.hpp"
#include "resource/AGPUBuffer.hpp"

namespace Graphic::Resource {
class PointGPUBuffer : public AGPUBuffer {
  public:
    explicit PointGPUBuffer(Engine::Entity entity) : _entity(entity) {}
    ~PointGPUBuffer() override = default;
    void Create(Engine::Core &core) override
    {

        auto meshComponent = _entity.TryGetComponent<Object::Component::Mesh>(core);

        if (!meshComponent)
        {
            throw Exception::NonexistentComponentError(
                "Cannot create a GPU buffer for an entity without a Mesh component.");
        }

        std::vector<float> pointData;
        for (uint64_t i = 0; i < meshComponent->vertices.size(); ++i)
        {
            pointData.push_back(meshComponent->vertices[i].x);
            pointData.push_back(meshComponent->vertices[i].y);
            pointData.push_back(meshComponent->vertices[i].z);
            pointData.push_back(meshComponent->normals[i].x);
            pointData.push_back(meshComponent->normals[i].y);
            pointData.push_back(meshComponent->normals[i].z);
            pointData.push_back(meshComponent->texCoords[i].x);
            pointData.push_back(meshComponent->texCoords[i].y);
        }

        wgpu::BufferDescriptor bufferDesc(wgpu::Default);
        bufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Vertex;
        bufferDesc.size = sizeof(float) * pointData.size();

        _buffer = core.GetResource<Context>().deviceContext.GetDevice()->createBuffer(bufferDesc);

        core.GetResource<Context>().queue->writeBuffer(_buffer, 0, pointData.data(), bufferDesc.size);

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

        auto &meshComponent = core.GetRegistry().get<Object::Component::Mesh>(_entity);

        // For now, we will not implement dynamic resizing of the buffer. As we should have a way to know if the size
        // changed. And it would be so heavy to check every frame every vertex position, normal and texCoord.
    };

  private:
    wgpu::Buffer _buffer;
    bool _isCreated = false;
    Engine::Entity _entity = Engine::Entity::entity_null_id;
};
} // namespace Graphic::Resource
