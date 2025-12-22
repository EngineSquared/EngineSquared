#pragma once

#include "entity/Entity.hpp"
#include "resource/AGPUBuffer.hpp"

namespace Graphic::Resource {

class CameraGPUBuffer final : public Graphic::Resource::AGPUBuffer {
  public:
    explicit CameraGPUBuffer(Engine::Entity entity) : _entity(entity) {}

    void Create(Engine::Core &core) override
    {
        wgpu::BufferDescriptor bufferDesc(wgpu::Default);
        std::string label =
            "CameraUniformBuffer_" + Log::EntityToDebugString(static_cast<Engine::Entity::entity_id_type>(_entity));
        bufferDesc.label = wgpu::StringView(label);
        bufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform;
        bufferDesc.size = sizeof(glm::mat4);

        _buffer = core.GetResource<Graphic::Resource::Context>().deviceContext.GetDevice()->createBuffer(bufferDesc);

        const auto &gpuCamera = _entity.GetComponents<Graphic::Component::GPUCamera>(core);
        core.GetResource<Graphic::Resource::Context>().queue->writeBuffer(_buffer, 0, &gpuCamera.viewProjection,
                                                                          sizeof(glm::mat4));

        _isCreated = true;
    }

    void Destroy(Engine::Core &) override
    {
        if (_isCreated)
        {
            _isCreated = false;
            _buffer.release();
        }
    }

    bool IsCreated(Engine::Core &) const override { return _isCreated; }

    void Update(Engine::Core &core) override
    {
        if (_isCreated)
        {
            const auto &gpuCamera = _entity.GetComponents<Graphic::Component::GPUCamera>(core);
            core.GetResource<Graphic::Resource::Context>().queue->writeBuffer(_buffer, 0, &gpuCamera.viewProjection,
                                                                              sizeof(glm::mat4));
        }
    }

    const wgpu::Buffer &GetBuffer() const override { return _buffer; }

  private:
    Engine::Entity _entity;
    bool _isCreated = false;
    wgpu::Buffer _buffer;
};
} // namespace Graphic::Resource
