#pragma once

#include "component/Mesh.hpp"
#include "entity/Entity.hpp"
#include "exception/NonexistentComponentError.hpp"
#include "exception/UpdateBufferError.hpp"
#include "resource/AGPUBuffer.hpp"

namespace DefaultPipeline::Resource {
class PointGPUBuffer : public Graphic::Resource::AGPUBuffer {
  public:
    explicit PointGPUBuffer(Engine::Entity entity) : _entity(entity) {}
    ~PointGPUBuffer() override = default;
    void Create(Engine::Core &core) override
    {

        auto meshComponent = _entity.TryGetComponent<Object::Component::Mesh>();

        if (!meshComponent)
        {
            throw Graphic::Exception::NonexistentComponentError(
                "Cannot create a GPU buffer for an entity without a Mesh component.");
        }

        std::vector<float> pointData;

        pointData.reserve(meshComponent->vertices.size() * 8);

        for (uint64_t i = 0; i < meshComponent->vertices.size(); ++i)
        {
            pointData.emplace_back(meshComponent->vertices[i].x);
            pointData.emplace_back(meshComponent->vertices[i].y);
            pointData.emplace_back(meshComponent->vertices[i].z);
            pointData.emplace_back(meshComponent->normals[i].x);
            pointData.emplace_back(meshComponent->normals[i].y);
            pointData.emplace_back(meshComponent->normals[i].z);
            pointData.emplace_back(meshComponent->texCoords[i].x);
            pointData.emplace_back(meshComponent->texCoords[i].y);
        }

        wgpu::BufferDescriptor bufferDesc(wgpu::Default);
        bufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Vertex;
        bufferDesc.size = sizeof(float) * pointData.size();
        std::string label = fmt::format("PointGPUBuffer_{}", _entity);
        bufferDesc.label = wgpu::StringView(label);

        const auto &context = core.GetResource<Graphic::Resource::Context>();
        _buffer = context.deviceContext.GetDevice()->createBuffer(bufferDesc);

        context.queue->writeBuffer(_buffer, 0, pointData.data(), bufferDesc.size);

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

        const auto &meshComponent = _entity.GetComponents<Object::Component::Mesh>();

        // For now, we will not implement dynamic resizing of the buffer. As we should have a way to know if the size
        // changed. And it would be so heavy to check every frame every vertex position, normal and texCoord.
    };

    const wgpu::Buffer &GetBuffer() const override { return _buffer; };

  private:
    wgpu::Buffer _buffer;
    bool _isCreated = false;
    Engine::Entity _entity = Engine::Entity::Null();
};
} // namespace DefaultPipeline::Resource
