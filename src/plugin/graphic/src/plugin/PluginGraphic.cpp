#include "plugin/PluginGraphic.hpp"
#include "Graphic.hpp"
#include "RenderingPipeline.hpp"
#include "component/Material.hpp"
#include "plugin/PluginWindow.hpp"
#include "scheduler/Shutdown.hpp"
#include <iomanip>
#include <sstream>

void Graphic::Plugin::Bind()
{
    RequirePlugins<RenderingPipeline::Plugin>();

    RegisterResource(Graphic::Resource::Context());
    RegisterResource(Graphic::Resource::GraphicSettings());
    RegisterResource(Graphic::Resource::ShaderContainer());
    RegisterResource(Graphic::Resource::TextureContainer());
    RegisterResource(Graphic::Resource::GPUBufferContainer());
    RegisterResource(Graphic::Resource::SamplerContainer());
    RegisterResource(Graphic::Resource::BindGroupManager());
    RegisterResource(Graphic::Resource::RenderGraphContainer());

    // TODO: Implement disconnect on destroy
    this->GetCore().GetRegistry().on_construct<Object::Component::Camera>().connect<&Graphic::System::OnCameraCreation>(
        this->GetCore());
    this->GetCore().GetRegistry().on_construct<Object::Component::Mesh>().connect<&Graphic::System::OnMeshCreation>(
        this->GetCore());
    this->GetCore()
        .GetRegistry()
        .on_construct<Object::Component::Transform>()
        .connect<&Graphic::System::OnTransformCreation>(this->GetCore());
    this->GetCore()
        .GetRegistry()
        .on_construct<Object::Component::Material>()
        .connect<&Graphic::System::OnMaterialCreation>(this->GetCore());

    RegisterSystems<RenderingPipeline::Setup>(
        System::CreateInstance, System::CreateSurface, System::CreateAdapter, System::ReleaseInstance,
        System::RequestCapabilities, System::CreateDevice, System::CreateQueue, System::SetupQueue,
        System::ConfigureSurface, System::ReleaseAdapter, System::CreateDefaultRenderPipeline, System::CreateDefaultMaterial);

    RegisterSystems<RenderingPipeline::Preparation>(System::PrepareEndRenderTexture, System::UpdateGPUTransforms,
                                                    System::UpdateGPUCameras, System::UpdateGPUMaterials);

    RegisterSystems<RenderingPipeline::CommandCreation>(System::ExecuteRenderPass);

    RegisterSystems<RenderingPipeline::Presentation>(System::Present);

    RegisterSystems<Engine::Scheduler::Shutdown>(System::ReleaseGPUBuffer, System::ReleaseBindingGroup,
                                                 System::ReleaseShader, System::ReleaseTexture, System::ReleaseSampler,
                                                 System::ReleaseContext);
}
