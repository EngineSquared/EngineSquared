#include "system/GPUComponentManagement/OnTransformCreation.hpp"
#include "component/GPUTransform.hpp"
#include "component/Transform.hpp"
#include "resource/BindGroup.hpp"
#include "resource/BindGroupManager.hpp"
#include "resource/GPUBufferContainer.hpp"
#include "resource/buffer/TransformGPUBuffer.hpp"
#include <string>

void DefaultPipeline::System::OnTransformCreation(Engine::Core &core, Engine::Entity entity)
{
    const auto &transform = entity.GetComponents<Object::Component::Transform>(core);
    auto &GPUTransform = entity.AddComponent<Component::GPUTransform>(core);
    auto &gpuBufferContainer = core.GetResource<Graphic::Resource::GPUBufferContainer>();

    std::string entityString = Log::EntityToDebugString(static_cast<Engine::Entity::entity_id_type>(entity));

    std::string transformBufferName = "TRANSFORM_BUFFER_" + entityString;
    entt::hashed_string transformBufferId{transformBufferName.data(), transformBufferName.size()};
    gpuBufferContainer.Add(transformBufferId, std::make_unique<Resource::TransformGPUBuffer>(entity));
    gpuBufferContainer.Get(transformBufferId)->Create(core);
    GPUTransform.modelMatrixBuffer = transformBufferId;
    auto &bindGroupManager = core.GetResource<Graphic::Resource::BindGroupManager>();
    std::string bindGroupName = "TRANSFORM_BIND_GROUP_" + entityString;
    entt::hashed_string bindGroupId{bindGroupName.data(), bindGroupName.size()};
    Graphic::Resource::BindGroup bindGroup(core, "DEFAULT_RENDER_PASS_SHADER", 1,
                                           {
                                               {
                                                0, Graphic::Resource::BindGroup::Asset::Type::Buffer,
                                                transformBufferId, gpuBufferContainer.Get(transformBufferId)->GetBuffer().getSize(),
                                                },
    });
    bindGroupManager.Add(bindGroupId, std::move(bindGroup));
    GPUTransform.bindGroup = bindGroupId;
}
