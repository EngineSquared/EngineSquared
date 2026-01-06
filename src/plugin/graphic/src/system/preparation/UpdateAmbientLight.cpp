#include "UpdateAmbientLight.hpp"
#include "resource/AmbientLight.hpp"
#include "component/AmbientLight.hpp"
#include "entity/Entity.hpp"

namespace Graphic::System {

void UpdateAmbientLight(Engine::Core &core)
{
    auto &ambientLightResource = core.GetResource<Graphic::Resource::AmbientLight>();
    auto view = core.GetRegistry().view<Object::Component::AmbientLight>();
    if (view.empty())
        ambientLightResource.SetEntity(core, Engine::Entity{});
    else {
        Engine::Entity ambientLightEntity{view.front()};
        ambientLightResource.SetEntity(core, ambientLightEntity);
    }
    ambientLightResource.Update(core);
}

} // namespace Graphic::System
