#include "system/GPUComponentManagement/OnTransformCreation.hpp"
#include "component/GPUTransform.hpp"
#include "component/Transform.hpp"
#include "resource/BindGroup.hpp"
#include "resource/BindGroupManager.hpp"
#include "resource/GPUBufferContainer.hpp"
#include "resource/buffer/TransformGPUBuffer.hpp"
#include <string>

void Graphic::System::OnTransformCreation(Engine::Core &core, Engine::Entity entity)
{
    Graphic::Component::GPUTransform &GPUTransform = entity.AddComponent<Graphic::Component::GPUTransform>(core);
    const auto &transform = entity.GetComponents<Object::Component::Transform>(core);

    std::string entityString = Log::EntityToDebugString(static_cast<Engine::Entity::entity_id_type>(entity));

    auto &gpuBufferContainer = core.GetResource<Graphic::Resource::GPUBufferContainer>();

    std::string transformBufferName = "TRANSFORM_BUFFER_" + entityString;
    entt::hashed_string transformBufferId{transformBufferName.data(), transformBufferName.size()};
    gpuBufferContainer.Add(transformBufferId, std::make_unique<Graphic::Resource::TransformGPUBuffer>(entity));
    gpuBufferContainer.Get(transformBufferId)->Create(core);
    GPUTransform.modelMatrixBuffer = transformBufferId;
    auto &bindGroupManager = core.GetResource<Graphic::Resource::BindGroupManager>();
    std::string bindGroupName = "TRANSFORM_BIND_GROUP_" + entityString;
    entt::hashed_string bindGroupId{bindGroupName.data(), bindGroupName.size()};
    Resource::BindGroup bindGroup(core, "DEFAULT_RENDER_PASS_SHADER", 1,
                                  {
                                      {
                                       0, Resource::BindGroup::Asset::Type::Buffer,
                                       transformBufferId, gpuBufferContainer.Get(transformBufferId)->GetBuffer().getSize(),
                                       },
    });
    bindGroupManager.Add(bindGroupId, std::move(bindGroup));
    GPUTransform.bindGroup = bindGroupId;
}
