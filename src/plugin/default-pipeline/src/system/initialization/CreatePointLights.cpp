#include "system/initialization/CreatePointLights.hpp"
#include "resource/BindGroupManager.hpp"
#include "resource/GPUBufferContainer.hpp"
#include "resource/buffer/PointLightsBuffer.hpp"
#include "utils/AmbientLight.hpp"
#include "utils/DefaultRenderPass.hpp"

namespace DefaultPipeline::System {

void CreatePointLights(Engine::Core &core)
{
    auto &bufferManager = core.GetResource<Graphic::Resource::GPUBufferContainer>();

    auto pointLightsBuffer = std::make_unique<Resource::PointLightsBuffer>();
    pointLightsBuffer->Create(core);
    auto pointLightsBufferSize = pointLightsBuffer->GetBuffer().getSize();
    bufferManager.Add(Utils::POINT_LIGHTS_BUFFER_ID, std::move(pointLightsBuffer));

    auto &ambientLightBuffer = bufferManager.Get(Utils::AMBIENT_LIGHT_BUFFER_ID);
    auto ambientLightBufferSize = ambientLightBuffer->GetBuffer().getSize();

    auto &bindGroupManager = core.GetResource<Graphic::Resource::BindGroupManager>();
    Graphic::Resource::BindGroup lightsBindGroup(core, Utils::DEFAULT_RENDER_PASS_SHADER_ID, 3,
                                                 {
                                                     {0, Graphic::Resource::BindGroup::Asset::Type::Buffer,
                                                      Utils::AMBIENT_LIGHT_BUFFER_ID, ambientLightBufferSize},
                                                     {1, Graphic::Resource::BindGroup::Asset::Type::Buffer,
                                                      Utils::POINT_LIGHTS_BUFFER_ID,  pointLightsBufferSize }
    });
    bindGroupManager.Add(Utils::LIGHTS_BIND_GROUP_ID, lightsBindGroup);
}
} // namespace DefaultPipeline::System
