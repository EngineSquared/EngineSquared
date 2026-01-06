#include "CreateAmbientLight.hpp"
#include "utils/AmbientLight.hpp"
#include "resource/AmbientLight.hpp"
#include "resource/BindGroupManager.hpp"
#include "resource/buffer/AmbientLightBuffer.hpp"
#include "resource/GPUBufferContainer.hpp"

namespace Graphic::System {

void CreateAmbientLight(Engine::Core &core)
{
    auto &ambientLight = core.GetResource<Graphic::Resource::AmbientLight>();
    ambientLight.Create(core);
}

}
