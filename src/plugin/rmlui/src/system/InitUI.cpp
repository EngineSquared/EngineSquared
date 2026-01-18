#include "system/InitUI.hpp"

#include "core/Core.hpp"
#include "resource/UIContext.hpp"
#include "utils/RenderInterface.hpp"
#include "utils/SystemInterface.hpp"

void Rmlui::System::Init(Engine::Core &core)
{
    core.GetResource<Rmlui::Resource::UIContext>().Init<Rmlui::Utils::SystemInterface, Rmlui::Utils::RenderInterface>(
        core);
}

void Rmlui::System::Destroy(Engine::Core &core)
{
    core.GetResource<Rmlui::Resource::UIContext>().Destroy(core);
}
