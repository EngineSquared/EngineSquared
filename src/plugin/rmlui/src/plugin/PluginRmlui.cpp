#include "plugin/PluginRmlui.hpp"

#include "plugin/PluginGraphic.hpp"
#include "plugin/PluginInput.hpp"
#include "plugin/PluginRenderingPipeline.hpp"
#include "plugin/PluginWindow.hpp"

#include "resource/UIContext.hpp"

#include "scheduler/PreUpdate.hpp"
#include "scheduler/Preparation.hpp"
#include "scheduler/Setup.hpp"
#include "scheduler/Shutdown.hpp"

#include "system/BindInputCallbacks.hpp"
#include "system/EventSystems.hpp"
#include "system/InitUI.hpp"
#include "system/UpdateUI.hpp"
#include "system/CreateRmluiRenderPipeline.hpp"

void Rmlui::Plugin::Bind()
{
    RequirePlugins<RenderingPipeline::Plugin, Window::Plugin, Input::Plugin, Graphic::Plugin>();

    RegisterResource<Resource::UIContext>(Resource::UIContext());

    RegisterSystems<RenderingPipeline::Setup>(System::CreateRmluiRenderPipeline, System::BindInputCallbacks,
                                              System::Init);
    RegisterSystems<RenderingPipeline::PreUpdate>(System::UpdateMouseMoveEvent, System::Update);
    RegisterSystems<RenderingPipeline::Preparation>(System::Render);
    RegisterSystems<Engine::Scheduler::Shutdown>(System::Destroy);
}
