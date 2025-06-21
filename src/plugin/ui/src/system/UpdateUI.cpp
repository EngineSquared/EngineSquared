#include "UpdateUI.hpp"
#include "UIResource.hpp"

void ES::Plugin::UI::System::Update(ES::Engine::Core &core)
{
    core.GetResource<ES::Plugin::UI::Resource::UIResource>().Update(core);
}

void ES::Plugin::UI::System::Render(ES::Engine::Core &core)
{
    core.GetResource<ES::Plugin::UI::Resource::UIResource>().Render();
}
