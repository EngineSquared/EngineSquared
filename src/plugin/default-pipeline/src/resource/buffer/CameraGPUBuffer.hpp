#pragma once

#include "component/GPUCamera.hpp"
#include "entity/Entity.hpp"
#include "exception/UpdateBufferError.hpp"
#include "resource/AGPUBuffer.hpp"
#include <glm/gtc/type_ptr.hpp>

namespace DefaultPipeline::Resource {

class CameraGPUBuffer final : public Graphic::Resource::AGPUBuffer {
  public:
    explicit CameraGPUBuffer(Engine::Entity entity) : _entity(entity) {}

    void Create(Engine::Core &core) override
    {
        const auto &gpuCamera = _entity.GetComponents<DefaultPipeline::Component::GPUCamera>(core);
        const auto &context = core.GetResource<Graphic::Resource::Context>();

        _buffer = _CreateBuffer(context.deviceContext);
        _UpdateBuffer(gpuCamera, context);

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
        if (!_isCreated)
        {
            throw Graphic::Exception::UpdateBufferError("Cannot update a GPU camera buffer that is not created.");
        }
        const auto &cameraComponent = _entity.GetComponents<Component::GPUCamera>(core);
        const auto &context = core.GetResource<Graphic::Resource::Context>();
        _UpdateBuffer(cameraComponent, context);
    }

    const wgpu::Buffer &GetBuffer() const override { return _buffer; }

  private:
    wgpu::Buffer _CreateBuffer(const Graphic::Resource::DeviceContext &context)
    {
        wgpu::BufferDescriptor bufferDesc(wgpu::Default);
        std::string label =
            "CameraUniformBuffer_" + Log::EntityToDebugString(static_cast<Engine::Entity::entity_id_type>(_entity));
        bufferDesc.label = wgpu::StringView(label);
        bufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform;
        bufferDesc.size = sizeof(glm::mat4);

        return context.GetDevice()->createBuffer(bufferDesc);
    }

    void _UpdateBuffer(const Component::GPUCamera &GPUCameraComponent, const Graphic::Resource::Context &context)
    {
        const glm::mat4 &viewProjection = GPUCameraComponent.viewProjection;
        context.queue->writeBuffer(_buffer, 0, glm::value_ptr(viewProjection), sizeof(glm::mat4));
    }

    Engine::Entity _entity;
    bool _isCreated = false;
    wgpu::Buffer _buffer;
};
} // namespace DefaultPipeline::Resource
