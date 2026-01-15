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

        auto meshComponent = _entity.TryGetComponent<Object::Component::Mesh>(core);

        if (!meshComponent)
        {
            throw Graphic::Exception::NonexistentComponentError(
                "Cannot create a GPU buffer for an entity without a Mesh component.");
        }

        if (meshComponent->normals.size() != meshComponent->vertices.size() ||
            meshComponent->texCoords.size() != meshComponent->vertices.size())
        {
            throw Graphic::Exception::UpdateBufferError(
                "Cannot create GPU buffer: normals or texCoords size mismatch with vertices.");
        }

        wgpu::BufferDescriptor bufferDesc(wgpu::Default);
        bufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Vertex;
        bufferDesc.size = sizeof(float) * meshComponent->vertices.size() * 8;
        std::string label =
            "PointGPUBuffer_" + Log::EntityToDebugString(static_cast<Engine::Entity::entity_id_type>(_entity));
        bufferDesc.label = wgpu::StringView(label);

        const auto &context = core.GetResource<Graphic::Resource::Context>();
        _buffer = context.deviceContext.GetDevice()->createBuffer(bufferDesc);

        _isCreated = true;
        Update(core);
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

        const auto &meshComponent = _entity.GetComponents<Object::Component::Mesh>(core);
        if (meshComponent.vertices.empty())
        {
            throw Graphic::Exception::UpdateBufferError(
                "Cannot update a GPU buffer from a Mesh component with no vertices.");
        }

        if (meshComponent.normals.size() != meshComponent.vertices.size() ||
            meshComponent.texCoords.size() != meshComponent.vertices.size())
        {
            throw Graphic::Exception::UpdateBufferError(
                "Cannot update GPU buffer: normals or texCoords size mismatch with vertices.");
        }

        std::vector<float> pointData;
        pointData.reserve(meshComponent.vertices.size() * 8u);

        for (uint64_t i = 0u; i < meshComponent.vertices.size(); ++i)
        {
            pointData.emplace_back(meshComponent.vertices[i].x);
            pointData.emplace_back(meshComponent.vertices[i].y);
            pointData.emplace_back(meshComponent.vertices[i].z);
            pointData.emplace_back(meshComponent.normals[i].x);
            pointData.emplace_back(meshComponent.normals[i].y);
            pointData.emplace_back(meshComponent.normals[i].z);
            pointData.emplace_back(meshComponent.texCoords[i].x);
            pointData.emplace_back(meshComponent.texCoords[i].y);
        }

        core.GetResource<Graphic::Resource::Context>().queue->writeBuffer(_buffer, 0, pointData.data(),
                                                                          sizeof(float) * pointData.size());
    };

    const wgpu::Buffer &GetBuffer() const override { return _buffer; };

  private:
    wgpu::Buffer _buffer;
    bool _isCreated = false;
    Engine::Entity _entity = Engine::Entity::entity_null_id;
};
} // namespace DefaultPipeline::Resource
