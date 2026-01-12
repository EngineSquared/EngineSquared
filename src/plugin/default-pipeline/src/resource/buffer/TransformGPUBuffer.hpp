#pragma once

#include "component/Mesh.hpp"
#include "component/Transform.hpp"
#include "entity/Entity.hpp"
#include "exception/NonexistentComponentError.hpp"
#include "exception/UpdateBufferError.hpp"
#include "resource/AGPUBuffer.hpp"
#include <glm/gtc/type_ptr.hpp>

namespace DefaultPipeline::Resource {

/**
 * @brief GPU buffer structure for model transform data
 *
 * Contains the model matrix and normal matrix for proper vertex/normal transformations.
 * The normal matrix (inverse transpose of the upper-left 3x3 of modelMatrix) is required
 * for correct normal transformation when the model has non-uniform scaling.
 *
 * Layout (WGSL std140 alignment):
 * - modelMatrix: mat4x4<f32> (64 bytes, offset 0)
 * - normalMatrix: mat3x3<f32> (48 bytes, offset 64) - each column is 16-byte aligned
 * Total: 112 bytes
 */
struct TransformGPUData {
    glm::mat4 modelMatrix;
    // mat3x3 in WGSL has each column aligned to 16 bytes, so we use vec4 for each column
    glm::vec4 normalMatrixCol0;
    glm::vec4 normalMatrixCol1;
    glm::vec4 normalMatrixCol2;
};

class TransformGPUBuffer : public Graphic::Resource::AGPUBuffer {
  public:
    explicit TransformGPUBuffer(Engine::Entity entity) : _entity(entity) {}
    ~TransformGPUBuffer() override = default;
    void Create(Engine::Core &core) override
    {
        const auto &transformComponent = _entity.GetComponents<Object::Component::Transform>(core);
        const auto &context = core.GetResource<Graphic::Resource::Context>();

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
            throw Graphic::Exception::UpdateBufferError("Cannot update a GPU transform buffer that is not created.");
        }

        const auto &transformComponent = _entity.GetComponents<Object::Component::Transform>(core);
        const auto &context = core.GetResource<Graphic::Resource::Context>();
        _UpdateBuffer(transformComponent, context);
    };

    const wgpu::Buffer &GetBuffer() const override { return _buffer; };

  private:
    wgpu::Buffer _CreateBuffer(const Graphic::Resource::DeviceContext &context)
    {
        wgpu::BufferDescriptor bufferDesc(wgpu::Default);
        bufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform;
        bufferDesc.size = sizeof(TransformGPUData);
        const std::string label =
            "TransformGPUBuffer_" + Log::EntityToDebugString(static_cast<Engine::Entity::entity_id_type>(_entity));
        bufferDesc.label = wgpu::StringView(label);

        return context.GetDevice()->createBuffer(bufferDesc);
    }

    void _UpdateBuffer(const Object::Component::Transform &transformComponent,
                       const Graphic::Resource::Context &context)
    {
        const glm::mat4 &modelMatrix = transformComponent.ComputeTransformationMatrix();

        const glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));

        TransformGPUData gpuData;
        gpuData.modelMatrix = modelMatrix;
        gpuData.normalMatrixCol0 = glm::vec4(normalMatrix[0], 0.0f);
        gpuData.normalMatrixCol1 = glm::vec4(normalMatrix[1], 0.0f);
        gpuData.normalMatrixCol2 = glm::vec4(normalMatrix[2], 0.0f);

        context.queue->writeBuffer(_buffer, 0, &gpuData, sizeof(TransformGPUData));
    }

    wgpu::Buffer _buffer;
    bool _isCreated = false;
    Engine::Entity _entity = Engine::Entity::entity_null_id;
};
} // namespace DefaultPipeline::Resource
