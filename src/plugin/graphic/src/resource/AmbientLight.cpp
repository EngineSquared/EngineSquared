#include "AmbientLight.hpp"
#include "resource/BindGroupManager.hpp"
#include "resource/GPUBufferContainer.hpp"
#include "resource/buffer/AmbientLightBuffer.hpp"
#include "utils/DefaultPipeline.hpp"

namespace Graphic::Resource {

void AmbientLight::Create(Engine::Core &core)
{
    auto &bufferManager = core.GetResource<GPUBufferContainer>();
    auto ambientLightBuffer = std::make_unique<AmbientLightBuffer>();
    ambientLightBuffer->Create(core);
    Object::Component::AmbientLight ambientLightComponent;
    ambientLightBuffer->SetValue(core, ambientLightComponent);
    auto bufferSize = ambientLightBuffer->GetBuffer().getSize();
    bufferManager.Add(Utils::AMBIENT_LIGHT_BUFFER_ID, std::move(ambientLightBuffer));

    auto &bindGroupManager = core.GetResource<Resource::BindGroupManager>();
    Graphic::Resource::BindGroup ambientLightBindGroup(
        core, Graphic::Utils::DEFAULT_RENDER_PASS_SHADER_ID, 3,
        {
            {0, Graphic::Resource::BindGroup::Asset::Type::Buffer, Utils::AMBIENT_LIGHT_BUFFER_ID, bufferSize}
    });
    bindGroupManager.Add(Utils::AMBIENT_LIGHT_BIND_GROUP_ID, ambientLightBindGroup);
}

void AmbientLight::Update(Engine::Core &core)
{
    auto &lightBuffer = GetLight(core);
    lightBuffer.Update(core);
}

void AmbientLight::SetEntity(Engine::Core &core, Engine::Entity entity)
{
    auto &lightBuffer = GetLight(core);
    lightBuffer.SetEntity(entity);
}

void AmbientLight::SetValue(Engine::Core &core, const Object::Component::AmbientLight &ambientLight)
{
    auto &lightBuffer = GetLight(core);
    lightBuffer.SetValue(core, ambientLight);
}

AmbientLightBuffer &AmbientLight::GetLight(Engine::Core &core)
{
    auto &bufferManager = core.GetResource<GPUBufferContainer>();
    auto &ambientLightBuffer = bufferManager.Get(Utils::AMBIENT_LIGHT_BUFFER_ID);
    auto ambientLightBufferPtr = dynamic_cast<AmbientLightBuffer *>(ambientLightBuffer.get());
    if (!ambientLightBufferPtr)
    {
        throw Exception::UpdateBufferError("Failed to cast AGPUBuffer to AmbientLightBuffer.");
    }
    return *ambientLightBufferPtr;
}

} // namespace Graphic::Resource
