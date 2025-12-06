#include "plugin/PluginGraphic.hpp"
#include "Graphic.hpp"
#include "RenderingPipeline.hpp"
#include "scheduler/Shutdown.hpp"

void Graphic::Plugin::Bind()
{
    RequirePlugins<RenderingPipeline::Plugin>();

    RegisterResource(Graphic::Resource::Context());
    RegisterResource(Graphic::Resource::GraphicSettings());
    RegisterResource(Graphic::Resource::ShaderContainer());
    RegisterResource(Graphic::Resource::TextureContainer());
    RegisterResource(Graphic::Resource::GPUBufferContainer());
    RegisterResource(Graphic::Resource::SamplerContainer());

    RegisterSystems<RenderingPipeline::Setup>(
        System::CreateInstance, System::CreateSurface, System::CreateAdapter, System::ReleaseInstance,
        System::RequestCapabilities, System::CreateDevice, System::CreateQueue, System::SetupQueue,
        System::ConfigureSurface, System::CreateRenderSurface, System::ReleaseAdapter);

    RegisterSystems<Engine::Scheduler::Shutdown>(System::ReleaseContext);
}
