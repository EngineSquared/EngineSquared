#include "system/EventSystems.hpp"

#include "resource/UIContext.hpp"

void Rmlui::System::UpdateMouseMoveEvent(Engine::Core &core)
{
    core.GetResource<Rmlui::Resource::UIContext>().UpdateMouseMoveEvent(core);
}
