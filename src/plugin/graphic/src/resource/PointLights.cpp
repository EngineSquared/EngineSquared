#include "PointLights.hpp"
#include "resource/BindGroupManager.hpp"
#include "resource/GPUBufferContainer.hpp"
#include "resource/buffer/PointLightsBuffer.hpp"
#include "utils/AmbientLight.hpp"
#include "utils/DefaultPipeline.hpp"
#include "utils/PointLight.hpp"

namespace Graphic::Resource {

void PointLights::Create(Engine::Core &core)
{
    auto &bufferManager = core.GetResource<GPUBufferContainer>();

    auto pointLightsBuffer = std::make_unique<PointLightsBuffer>();
    pointLightsBuffer->Create(core);
    auto pointLightsBufferSize = pointLightsBuffer->GetBuffer().getSize();
    bufferManager.Add(Utils::POINT_LIGHTS_BUFFER_ID, std::move(pointLightsBuffer));

    auto &ambientLightBuffer = bufferManager.Get(Utils::AMBIENT_LIGHT_BUFFER_ID);
    auto ambientLightBufferSize = ambientLightBuffer->GetBuffer().getSize();

    auto &bindGroupManager = core.GetResource<Resource::BindGroupManager>();
    Graphic::Resource::BindGroup lightsBindGroup(core, Graphic::Utils::DEFAULT_RENDER_PASS_SHADER_ID, 3,
                                                 {
                                                     {0, Graphic::Resource::BindGroup::Asset::Type::Buffer,
                                                      Utils::AMBIENT_LIGHT_BUFFER_ID, ambientLightBufferSize},
                                                     {1, Graphic::Resource::BindGroup::Asset::Type::Buffer,
                                                      Utils::POINT_LIGHTS_BUFFER_ID,  pointLightsBufferSize }
    });
    bindGroupManager.Add(Utils::LIGHTS_BIND_GROUP_ID, lightsBindGroup);
}

void PointLights::Update(Engine::Core &core)
{
    auto &bufferManager = core.GetResource<GPUBufferContainer>();
    bufferManager.Get(Utils::POINT_LIGHTS_BUFFER_ID)->Update(core);
}

} // namespace Graphic::Resource
