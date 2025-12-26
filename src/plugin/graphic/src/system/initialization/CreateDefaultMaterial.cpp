#include "system/initialization/CreateDefaultMaterial.hpp"
#include "component/GPUMaterial.hpp"
#include "component/Material.hpp"
#include "entity/Entity.hpp"
#include "resource/AGPUBuffer.hpp"
#include "resource/BindGroup.hpp"
#include "resource/BindGroupManager.hpp"
#include "resource/Context.hpp"
#include "resource/GPUBufferContainer.hpp"
#include "resource/Sampler.hpp"
#include "resource/SamplerContainer.hpp"
#include "resource/Texture.hpp"
#include "resource/TextureContainer.hpp"
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

    // TODO: Put this in a different system
    Resource::TextureContainer &textureContainer = core.GetResource<Resource::TextureContainer>();
    Resource::Texture emptyTexture(context, "EMPTY_TEXTURE", glm::uvec2(2, 2), [](glm::uvec2 pos) -> glm::u8vec4 {
        glm::u8vec4 color;
        color.r = ((pos.x + pos.y) % 2 == 0) ? 255 : 0;
        color.g = 0;
        color.b = ((pos.x + pos.y) % 2 == 0) ? 255 : 0;
        color.a = 255;
        return color;
    });
    textureContainer.SetDefault(std::move(emptyTexture));
    std::string defaultTextureName = "DEFAULT_TEXTURE";
    entt::hashed_string defaultTextureId{defaultTextureName.data(), defaultTextureName.size()};
    Resource::Texture defaultTexture(context, defaultTextureName, glm::uvec2(2, 2), [](glm::uvec2 pos) -> glm::u8vec4 {
        glm::u8vec4 color(150, 100, 100, 255);
        return color;
    });
    textureContainer.Add(defaultTextureId, std::move(defaultTexture));

    defaultMaterial.ambientTexName = "DEFAULT_TEXTURE";
    materialBuffer->SetMaterial(core, defaultMaterial);

    std::string defaultSamplerName = "DEFAULT_LINEAR_SAMPLER";
    entt::hashed_string defaultSamplerId{defaultSamplerName.data(), defaultSamplerName.size()};
    Resource::SamplerContainer &samplerContainer = core.GetResource<Resource::SamplerContainer>();
    Resource::Sampler defaultSampler(context.deviceContext.GetDevice().value());
    samplerContainer.Add(defaultSamplerId, std::move(defaultSampler));

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
                                      {1, Resource::BindGroup::Asset::Type::Texture, defaultTextureId, 0},
                                      {2, Resource::BindGroup::Asset::Type::Sampler, defaultSamplerId, 0},
    });
    bindGroupManager.Add(bindGroupId, std::move(bindGroup));
}
