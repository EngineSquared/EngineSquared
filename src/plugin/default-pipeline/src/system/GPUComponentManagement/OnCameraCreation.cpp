#include "system/GPUComponentManagement/OnCameraCreation.hpp"
#include "component/Camera.hpp"
#include "component/GPUCamera.hpp"
#include "component/Transform.hpp"
#include "resource/AGPUBuffer.hpp"
#include "resource/BindGroup.hpp"
#include "resource/BindGroupManager.hpp"
#include "resource/Context.hpp"
#include "resource/GPUBufferContainer.hpp"
#include "resource/buffer/CameraGPUBuffer.hpp"
#include "system/preparation/PrepareEndRenderTexture.hpp"
#include "utils/DefaultRenderPass.hpp"
#include "utils/EndRenderTexture.hpp"
#include <string>

void DefaultPipeline::System::OnCameraCreation(Engine::Core &core, Engine::Entity entity)
{
    auto &registry = core.GetRegistry();
    const auto &cameraComponent = entity.GetComponents<Object::Component::Camera>(core);
    const auto &transformComponent = entity.GetComponents<Object::Component::Transform>(core);

    Component::GPUCamera gpuCameraComponent;
    gpuCameraComponent.Update(cameraComponent, transformComponent);
    gpuCameraComponent.pipeline = Utils::DEFAULT_RENDER_GRAPH_ID;
    gpuCameraComponent.targetTexture = Graphic::Utils::END_RENDER_TEXTURE_ID;

    std::string entityString = Log::EntityToDebugString(static_cast<Engine::Entity::entity_id_type>(entity));
    std::string cameraBufferName = "CAMERA_UNIFORM_BUFFER_" + entityString;
    entt::hashed_string cameraBufferId{cameraBufferName.data(), cameraBufferName.size()};

    auto &gpuBufferContainer = core.GetResource<Graphic::Resource::GPUBufferContainer>();
    const auto &cameraUniformBuffer =
        gpuBufferContainer.Add(cameraBufferId, std::make_unique<Resource::CameraGPUBuffer>(entity));
    std::string bindGroupName = "CAMERA_BIND_GROUP_" + entityString;
    entt::hashed_string bindGroupId{bindGroupName.data(), bindGroupName.size()};
    entt::hashed_string shaderId = Utils::DEFAULT_RENDER_PASS_SHADER_ID;

    auto &gpuCameraComponentInContainer =
        entity.AddComponent<Component::GPUCamera>(core, std::move(gpuCameraComponent));
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
