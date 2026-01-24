#pragma once

#include "component/GPUCamera.hpp"
#include "component/Transform.hpp"
#include "entity/Entity.hpp"
#include "exception/UpdateBufferError.hpp"
#include "resource/AGPUBuffer.hpp"
#include <glm/gtc/type_ptr.hpp>

namespace DefaultPipeline::Resource {

class CameraGPUBuffer final : public Graphic::Resource::AGPUBuffer {

  public:
    struct CameraTransfer {
        glm::mat4 viewProjectionMatrix;
        glm::mat4 invViewProjectionMatrix;
        glm::vec3 position;

        explicit CameraTransfer(const Component::GPUCamera &gpuCamera, const Object::Component::Transform &transform)
            : viewProjectionMatrix(gpuCamera.viewProjection), invViewProjectionMatrix(gpuCamera.inverseViewProjection),
              position(transform.GetPosition())
        {
        }

        static uint32_t CPUSize() { return sizeof(CameraTransfer); }
        static uint32_t GPUSize() { return sizeof(CameraTransfer) + sizeof(float); }
    };

    explicit CameraGPUBuffer(Engine::Entity entity) : _entity(entity) {}
    ~CameraGPUBuffer() override { Destroy(); }

    void Create(Engine::Core &core) override
    {
        const auto &gpuCamera = _entity.GetComponents<Component::GPUCamera>();
        const auto &transform = _entity.GetComponents<Object::Component::Transform>();
        const auto &context = core.GetResource<Graphic::Resource::Context>();

        _buffer = _CreateBuffer(context.deviceContext);
        _UpdateBuffer(gpuCamera, transform, context);
        _isCreated = true;
    }

    void Destroy(Engine::Core &) override { Destroy(); }

    void Destroy()
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
        const auto &cameraComponent = _entity.GetComponents<Component::GPUCamera>();
        const auto &transform = _entity.GetComponents<Object::Component::Transform>();
        const auto &context = core.GetResource<Graphic::Resource::Context>();
        _UpdateBuffer(cameraComponent, transform, context);
    }

    const wgpu::Buffer &GetBuffer() const override { return _buffer; }

  private:
    wgpu::Buffer _CreateBuffer(const Graphic::Resource::DeviceContext &context)
    {
        wgpu::BufferDescriptor bufferDesc(wgpu::Default);
        std::string label = fmt::format("CameraUniformBuffer_{}", _entity);
        bufferDesc.label = wgpu::StringView(label);
        bufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform;
        bufferDesc.size = CameraTransfer::GPUSize();

        return context.GetDevice()->createBuffer(bufferDesc);
    }

    void _UpdateBuffer(const Component::GPUCamera &GPUCameraComponent, const Object::Component::Transform &transform,
                       const Graphic::Resource::Context &context)
    {
        const CameraTransfer cameraTransfer(GPUCameraComponent, transform);
        context.queue->writeBuffer(_buffer, 0, std::addressof(cameraTransfer), CameraTransfer::CPUSize());
    }

    Engine::Entity _entity;
    bool _isCreated = false;
    wgpu::Buffer _buffer;
};
} // namespace DefaultPipeline::Resource
