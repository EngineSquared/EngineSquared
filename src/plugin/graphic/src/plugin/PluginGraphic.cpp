#include "plugin/PluginGraphic.hpp"
#include "Graphic.hpp"
#include "RenderingPipeline.hpp"
#include "component/Material.hpp"
#include "plugin/PluginEvent.hpp"
#include "plugin/PluginWindow.hpp"
#include "scheduler/Shutdown.hpp"
#include <iomanip>
#include <sstream>

void Graphic::Plugin::Bind()
{
    RequirePlugins<RenderingPipeline::Plugin, Event::Plugin>();

    RegisterResource(Graphic::Resource::Context());
    RegisterResource(Graphic::Resource::GraphicSettings());
    RegisterResource(Graphic::Resource::ShaderContainer());
    RegisterResource(Graphic::Resource::TextureContainer());
    RegisterResource(Graphic::Resource::GPUBufferContainer());
    RegisterResource(Graphic::Resource::SamplerContainer());
    RegisterResource(Graphic::Resource::BindGroupManager());
    RegisterResource(Graphic::Resource::RenderGraphContainer());

    RegisterSystems<RenderingPipeline::Setup>(
        System::CreateInstance, System::CreateSurface, System::CreateAdapter, System::ReleaseInstance,
        System::RequestCapabilities, System::CreateDevice, System::CreateQueue, System::SetupQueue,
        System::ConfigureSurface, System::ReleaseAdapter, System::CreateEmptyTexture, System::CreateDefaultTexture,
        System::CreateDefaultSampler, System::SetupResizableRenderTexture);

    RegisterSystems<RenderingPipeline::Preparation>(System::PrepareEndRenderTexture);

    RegisterSystems<RenderingPipeline::CommandCreation>(System::ExecuteRenderPass);

    RegisterSystems<RenderingPipeline::Presentation>(System::Present);

    RegisterSystems<Engine::Scheduler::Shutdown>(System::ReleaseGPUBuffer, System::ReleaseBindingGroup,
                                                 System::ReleaseShader, System::ReleaseTexture, System::ReleaseSampler,
                                                 System::ReleaseContext);
}
