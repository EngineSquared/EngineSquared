#include "system/preparation/UpdateAmbientLight.hpp"
#include "component/AmbientLight.hpp"
#include "entity/Entity.hpp"
#include "resource/AmbientLight.hpp"

namespace DefaultPipeline::System {

void UpdateAmbientLight(Engine::Core &core)
{
    auto &ambientLightResource = core.GetResource<Resource::AmbientLight>();
    auto view = core.GetRegistry().view<Object::Component::AmbientLight>();
    if (view.empty())
        ambientLightResource.SetEntity(core, Engine::Entity{});
    else
    {
        Engine::Entity ambientLightEntity{view.front()};
        ambientLightResource.SetEntity(core, ambientLightEntity);
    }
    ambientLightResource.Update(core);
}

} // namespace Graphic::System
