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
#include "resource/pass/GBuffer.hpp"
#include "system/preparation/PrepareEndRenderTexture.hpp"
#include "utils/DefaultRenderPass.hpp"
#include "utils/EndRenderTexture.hpp"
#include <string>

void DefaultPipeline::System::OnCameraCreation(Engine::Core &core, Engine::EntityId entityId)
{
    Engine::Entity entity{core, entityId};
    const auto &cameraComponent = entity.GetComponents<Object::Component::Camera>();
    const auto &transformComponent = entity.GetComponents<Object::Component::Transform>();

    Component::GPUCamera gpuCameraComponent;
    gpuCameraComponent.Update(cameraComponent, transformComponent);
    gpuCameraComponent.pipeline = Utils::DEFAULT_RENDER_GRAPH_ID;
    gpuCameraComponent.targetTexture = Graphic::Utils::END_RENDER_TEXTURE_ID;

    std::string cameraBufferName = fmt::format("CAMERA_UNIFORM_BUFFER_{}", entity);
    entt::hashed_string cameraBufferId{cameraBufferName.data(), cameraBufferName.size()};

    auto &gpuBufferContainer = core.GetResource<Graphic::Resource::GPUBufferContainer>();
    const auto &cameraUniformBuffer =
        gpuBufferContainer.Add(cameraBufferId, std::make_unique<Resource::CameraGPUBuffer>(entity));
    std::string bindGroupName = fmt::format("CAMERA_BIND_GROUP_{}", entity);
    entt::hashed_string bindGroupId{bindGroupName.data(), bindGroupName.size()};
    entt::hashed_string shaderId = Resource::GBUFFER_SHADER_ID;

    auto &gpuCameraComponentInContainer = entity.AddComponent<Component::GPUCamera>(std::move(gpuCameraComponent));
    gpuBufferContainer.Get(cameraBufferId)->Create(core);
    gpuCameraComponentInContainer.buffer = cameraBufferId;

    Graphic::Resource::BindGroup cameraBindGroup(
        core, bindGroupName, shaderId, 0,
        {
            {0, Graphic::Resource::BindGroup::Asset::Type::Buffer, cameraBufferId,
             cameraUniformBuffer->get()->GetBuffer().getSize()}
    });
    gpuCameraComponentInContainer.bindGroup = bindGroupId;
    core.GetResource<Graphic::Resource::BindGroupManager>().Add(bindGroupId, std::move(cameraBindGroup));
}
