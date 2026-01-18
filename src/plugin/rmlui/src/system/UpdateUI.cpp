#include "system/UpdateUI.hpp"

#include "core/Core.hpp"
#include "resource/UIContext.hpp"

void Rmlui::System::Update(Engine::Core &core)
{
    core.GetResource<Rmlui::Resource::UIContext>().Update(core);
}

void Rmlui::System::Render(Engine::Core &core)
{
    core.GetResource<Rmlui::Resource::UIContext>().Render(core);
}
