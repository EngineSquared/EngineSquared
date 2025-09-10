#include "system/InitUI.hpp"
#include "resource/UIResource.hpp"

void ES::Plugin::UI::System::Init(ES::Engine::Core &core)
{
    core.GetResource<ES::Plugin::UI::Resource::UIResource>().Init(core);
}

void ES::Plugin::UI::System::BindEventCallback(ES::Engine::Core &core)
{
    core.GetResource<ES::Plugin::UI::Resource::UIResource>().BindEventCallback(core);
}

void ES::Plugin::UI::System::Destroy(ES::Engine::Core &core)
{
    core.GetResource<ES::Plugin::UI::Resource::UIResource>().Destroy();
}
