#include "resource/BindGroupManager.hpp"
#include "resource/GPUBufferContainer.hpp"
#include "resource/buffer/PointLightsBuffer.hpp"
#include "resource/pass/Deferred.hpp"
#include "system/initialization/CreatePointLights.hpp"
#include "utils/AmbientLight.hpp"
#include "utils/DirectionalLights.hpp"
#include "utils/Lights.hpp"

namespace DefaultPipeline::System {

void CreateLights(Engine::Core &core)
{
    auto &bufferManager = core.GetResource<Graphic::Resource::GPUBufferContainer>();

    auto &ambientLightBuffer = bufferManager.Get(Utils::AMBIENT_LIGHT_BUFFER_ID);
    auto ambientLightBufferSize = ambientLightBuffer->GetBuffer().getSize();

    auto &pointLightsBuffer = bufferManager.Get(Utils::POINT_LIGHTS_BUFFER_ID);
    auto pointLightsBufferSize = pointLightsBuffer->GetBuffer().getSize();

    auto &directionalLightsBuffer = bufferManager.Get(Utils::DIRECTIONAL_LIGHTS_BUFFER_ID);
    auto directionalLightsBufferSize = directionalLightsBuffer->GetBuffer().getSize();

    auto &bindGroupManager = core.GetResource<Graphic::Resource::BindGroupManager>();
    Graphic::Resource::BindGroup lightsBindGroup(
        core, Utils::LIGHTS_BIND_GROUP_NAME, Resource::DEFERRED_SHADER_ID, 2,
        {
            {0, Graphic::Resource::BindGroup::Asset::Type::Buffer, Utils::AMBIENT_LIGHT_BUFFER_ID,
             ambientLightBufferSize     },
            {1, Graphic::Resource::BindGroup::Asset::Type::Buffer, Utils::POINT_LIGHTS_BUFFER_ID,
             pointLightsBufferSize      },
            {2, Graphic::Resource::BindGroup::Asset::Type::Buffer, Utils::DIRECTIONAL_LIGHTS_BUFFER_ID,
             directionalLightsBufferSize}
    });
    bindGroupManager.Add(Utils::LIGHTS_BIND_GROUP_ID, std::move(lightsBindGroup));
}
} // namespace DefaultPipeline::System
