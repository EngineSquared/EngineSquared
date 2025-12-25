#include "system/initialization/CreateDefaultMaterial.hpp"
#include "component/GPUMaterial.hpp"
#include "component/Material.hpp"
#include "entity/Entity.hpp"
#include "resource/AGPUBuffer.hpp"
#include "resource/BindGroup.hpp"
#include "resource/BindGroupManager.hpp"
#include "resource/Context.hpp"
#include "resource/GPUBufferContainer.hpp"
#include "resource/buffer/MaterialGPUBuffer.hpp"

void Graphic::System::CreateDefaultMaterial(Engine::Core &core)
{
    const Resource::Context &context = core.GetResource<Resource::Context>();
    auto &bindGroupManager = core.GetResource<Resource::BindGroupManager>();

    auto materialBuffer = std::make_unique<Resource::MaterialGPUBuffer>();
    materialBuffer->Create(core);
    uint64_t materialBufferSize = materialBuffer->GetBuffer().getSize();

    Object::Component::Material defaultMaterial;
    defaultMaterial.ambient = glm::vec3(0.5f, 0.5f, 0.5f);
    materialBuffer->SetMaterial(core, defaultMaterial);

    std::string materialBufferName = "DEFAULT_MATERIAL_BUFFER";
    entt::hashed_string materialBufferId{materialBufferName.data(), materialBufferName.size()};

    Resource::GPUBufferContainer &gpuBufferContainer = core.GetResource<Resource::GPUBufferContainer>();
    gpuBufferContainer.Add(materialBufferId, std::move(materialBuffer));

    std::string bindGroupName = "DEFAULT_MATERIAL_BIND_GROUP";
    entt::hashed_string bindGroupId{bindGroupName.data(), bindGroupName.size()};
    Resource::BindGroup bindGroup(core, "DEFAULT_RENDER_PASS_SHADER", 2,
                                  {
                                      {
                                       0, Resource::BindGroup::Asset::Type::Buffer,
                                       materialBufferId, materialBufferSize,
                                       },
    });
    bindGroupManager.Add(bindGroupId, std::move(bindGroup));
}
