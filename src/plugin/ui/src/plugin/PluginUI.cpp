#include "PluginUI.hpp"
#include "UIResource.hpp"
#include "InitUI.hpp"
#include "UpdateUI.hpp"
#include "RenderingPipeline.hpp"

void ES::Plugin::UI::Plugin::Bind()
{
    RequirePlugins<ES::Plugin::RenderingPipeline::Plugin>();

    RegisterResource<ES::Plugin::UI::Resource::UIResource>(ES::Plugin::UI::Resource::UIResource::UIResource());
    
    RegisterSystems<ES::Plugin::RenderingPipeline::Init>(ES::Plugin::UI::System::Init);
    RegisterSystems<ES::Plugin::RenderingPipeline::RenderSetup>(ES::Plugin::UI::System::Update);
    RegisterSystems<ES::Plugin::RenderingPipeline::Draw>(ES::Plugin::UI::System::Render);
    RegisterSystems<ES::Engine::Scheduler::Shutdown>(ES::Plugin::UI::System::Destroy);
}
