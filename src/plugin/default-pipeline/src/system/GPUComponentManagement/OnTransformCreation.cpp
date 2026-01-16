#include "system/GPUComponentManagement/OnTransformCreation.hpp"
#include "component/GPUTransform.hpp"
#include "component/Transform.hpp"
#include "resource/BindGroup.hpp"
#include "resource/BindGroupManager.hpp"
#include "resource/GPUBufferContainer.hpp"
#include "resource/buffer/TransformGPUBuffer.hpp"
#include <string>

void DefaultPipeline::System::OnTransformCreation(Engine::Core &core, Engine::EntityId entityId)
{
    Engine::Entity entity{core, entityId};
    const auto &transform = entity.GetComponents<Object::Component::Transform>();
    auto &GPUTransform = entity.AddComponent<Component::GPUTransform>();
    auto &gpuBufferContainer = core.GetResource<Graphic::Resource::GPUBufferContainer>();

    std::string transformBufferName = fmt::format("TRANSFORM_BUFFER_{}", entity);
    entt::hashed_string transformBufferId{transformBufferName.data(), transformBufferName.size()};
    gpuBufferContainer.Add(transformBufferId, std::make_unique<Resource::TransformGPUBuffer>(entity));
    gpuBufferContainer.Get(transformBufferId)->Create(core);
    GPUTransform.modelMatrixBuffer = transformBufferId;
    auto &bindGroupManager = core.GetResource<Graphic::Resource::BindGroupManager>();
    std::string bindGroupName = fmt::format("TRANSFORM_BIND_GROUP_{}", entity);
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
