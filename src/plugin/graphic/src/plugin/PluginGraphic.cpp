#include "plugin/PluginGraphic.hpp"
#include "RenderingPipeline.hpp"
#include "Graphic.hpp"

void Plugin::Graphic::Plugin::Bind()
{
    RequirePlugins<RenderingPipeline::Plugin>();

    RegisterResource(Graphic::Resource::Context());

    RegisterSystems<RenderingPipeline::Init>(Graphic::System::CreateInstance);
}
