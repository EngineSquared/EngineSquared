#include "CreateAmbientLight.hpp"
#include "resource/AmbientLight.hpp"
#include "resource/BindGroupManager.hpp"
#include "resource/GPUBufferContainer.hpp"
#include "resource/buffer/AmbientLightBuffer.hpp"
#include "utils/AmbientLight.hpp"

namespace Graphic::System {

void CreateAmbientLight(Engine::Core &core)
{
    auto &ambientLight = core.GetResource<Graphic::Resource::AmbientLight>();
    ambientLight.Create(core);
}

} // namespace Graphic::System
