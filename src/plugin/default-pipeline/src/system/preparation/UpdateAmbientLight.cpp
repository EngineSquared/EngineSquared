#include "system/preparation/UpdateAmbientLight.hpp"
#include "component/AmbientLight.hpp"
#include "entity/Entity.hpp"
#include "resource/AmbientLight.hpp"

namespace DefaultPipeline::System {

void UpdateAmbientLight(Engine::Core &core)
{
    auto &ambientLightResource = core.GetResource<Resource::AmbientLight>();
    auto view = core.GetRegistry().view<Object::Component::AmbientLight>();
    if (!view.empty())
    {
        Engine::Entity ambientLightEntity{core, view.front()};
        ambientLightResource.SetEntity(core, ambientLightEntity);
    }
    ambientLightResource.Update(core);
}

} // namespace DefaultPipeline::System
