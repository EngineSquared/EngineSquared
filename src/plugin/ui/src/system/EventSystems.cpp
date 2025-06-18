#include "EventSystems.hpp"
#include "UIResource.hpp"

void ES::Plugin::UI::System::UpdateMouseMoveEvent(ES::Engine::Core &core)
{
    core.GetResource<ES::Plugin::UI::Resource::UIResource>().UpdateMouseMoveEvent(core);
}