#include "system/initialization/CreateDefaultMaterial.hpp"
#include "component/GPUMaterial.hpp"
#include "component/Material.hpp"
#include "resource/BindGroup.hpp"
#include "resource/BindGroupManager.hpp"
#include "resource/Context.hpp"
#include "resource/GPUBufferContainer.hpp"
#include "resource/buffer/MaterialGPUBuffer.hpp"
#include "resource/pass/GBuffer.hpp"
#include "utils/DefaultMaterial.hpp"
// #include "utils/DefaultRenderPass.hpp"
#include "utils/DefaultSampler.hpp"
#include "utils/DefaultTexture.hpp"

void DefaultPipeline::System::CreateDefaultMaterial(Engine::Core &core)
{
    auto &bindGroupManager = core.GetResource<Graphic::Resource::BindGroupManager>();

    auto materialBuffer = std::make_unique<Resource::MaterialGPUBuffer>();
    materialBuffer->Create(core);
    Object::Component::Material defaultMaterial;
    defaultMaterial.ambient = glm::vec3(0.5f, 0.5f, 0.5f);
    defaultMaterial.ambientTexName = Graphic::Utils::DEFAULT_TEXTURE_NAME;
    materialBuffer->SetMaterial(core, defaultMaterial);

    uint64_t materialBufferSize = materialBuffer->GetBuffer().getSize();

    auto &gpuBufferContainer = core.GetResource<Graphic::Resource::GPUBufferContainer>();
    gpuBufferContainer.Add(Utils::DEFAULT_MATERIAL_ID, std::move(materialBuffer));

    Graphic::Resource::BindGroup bindGroup(
        core, Utils::DEFAULT_MATERIAL_BIND_GROUP_NAME, Resource::GBUFFER_SHADER_ID, 2,
        {
            {
             0, Graphic::Resource::BindGroup::Asset::Type::Buffer,
             Utils::DEFAULT_MATERIAL_ID,
             materialBufferSize, },
            {1, Graphic::Resource::BindGroup::Asset::Type::Texture, Graphic::Utils::DEFAULT_TEXTURE_ID, 0},
            {2, Graphic::Resource::BindGroup::Asset::Type::Sampler, Graphic::Utils::DEFAULT_SAMPLER_ID, 0},
    });
    bindGroupManager.Add(Utils::DEFAULT_MATERIAL_BIND_GROUP_ID, std::move(bindGroup));
}
