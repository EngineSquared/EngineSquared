#include "plugin/PluginGraphic.hpp"
#include "Graphic.hpp"
#include "RenderingPipeline.hpp"

void Plugin::Graphic::Plugin::Bind()
{
    RequirePlugins<RenderingPipeline::Plugin>();

    RegisterResource(Graphic::Resource::Context());
    RegisterResource(Graphic::Resource::GraphicSettings());

    RegisterSystems<RenderingPipeline::Setup>(System::CreateInstance, System::CreateSurface, System::CreateAdapter);
}
