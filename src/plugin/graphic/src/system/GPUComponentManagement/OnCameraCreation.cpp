#include "system/GPUComponentManagement/OnCameraCreation.hpp"
#include "component/Camera.hpp"
#include "component/GPUCamera.hpp"
#include "component/Transform.hpp"
#include "resource/AGPUBuffer.hpp"
#include "resource/BindGroup.hpp"
#include "resource/BindGroupManager.hpp"
#include "resource/Context.hpp"
#include "resource/GPUBufferContainer.hpp"
#include "utils/DefaultPipeline.hpp"
#include <string>

namespace {

class CameraUniformBuffer final : public Graphic::Resource::AGPUBuffer {
  public:
    explicit CameraUniformBuffer(Engine::Entity entity) : _entity(entity) {}

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
} // namespace

void Graphic::System::OnCameraCreation(Engine::Core &core, Engine::Entity entity)
{
    auto &registry = core.GetRegistry();
    const auto &cameraComponent = entity.GetComponents<Object::Component::Camera>(core);
    const auto &transformComponent = entity.GetComponents<Object::Component::Transform>(core);

    Graphic::Component::GPUCamera gpuCameraComponent;
    glm::vec3 forward = transformComponent.rotation * glm::vec3(0.0f, 0.0f, 1.0f);
    gpuCameraComponent.view =
        glm::lookAt(transformComponent.position, transformComponent.position + forward, cameraComponent.up);
    gpuCameraComponent.projection = glm::perspectiveLH_ZO(cameraComponent.fov, gpuCameraComponent.aspectRatio,
                                                          cameraComponent.nearPlane, cameraComponent.farPlane);
    gpuCameraComponent.viewProjection = gpuCameraComponent.projection * gpuCameraComponent.view;
    gpuCameraComponent.inverseViewProjection = glm::inverse(gpuCameraComponent.viewProjection);
    gpuCameraComponent.pipeline = Graphic::Utils::DEFAULT_RENDER_GRAPH_ID;

    std::string entityString = Log::EntityToDebugString(static_cast<Engine::Entity::entity_id_type>(entity));
    std::string cameraBufferName = "CAMERA_UNIFORM_BUFFER_" + entityString;
    entt::hashed_string cameraBufferId{cameraBufferName.data(), cameraBufferName.size()};

    auto &gpuBufferContainer = core.GetResource<Graphic::Resource::GPUBufferContainer>();
    const auto &cameraUniformBuffer =
        gpuBufferContainer.Add(cameraBufferId, std::make_unique<CameraUniformBuffer>(entity));
    std::string bindGroupName = "CAMERA_BIND_GROUP_" + entityString;
    entt::hashed_string bindGroupId{bindGroupName.data(), bindGroupName.size()};
    entt::hashed_string shaderId = Graphic::Utils::DEFAULT_RENDER_PASS_SHADER_ID;

    auto &gpuCameraComponentInContainer =
        entity.AddComponent<Graphic::Component::GPUCamera>(core, std::move(gpuCameraComponent));
    gpuBufferContainer.Get(cameraBufferId)->Create(core);
    gpuCameraComponentInContainer.buffer = cameraBufferId;

    Graphic::Resource::BindGroup cameraBindGroup(
        core, shaderId, 0,
        {
            {0, Graphic::Resource::BindGroup::Asset::Type::Buffer, cameraBufferId,
             cameraUniformBuffer->get()->GetBuffer().getSize()}
    });
    gpuCameraComponentInContainer.bindGroup = bindGroupId;
    core.GetResource<Graphic::Resource::BindGroupManager>().Add(bindGroupId, cameraBindGroup);
}
