#include "plugin/PluginGraphic.hpp"
#include "Graphic.hpp"
#include "RenderingPipeline.hpp"
#include "component/Material.hpp"
#include "plugin/PluginWindow.hpp"
#include "scheduler/Shutdown.hpp"
#include <iomanip>
#include <sstream>

template <typename CPUComponent, typename GPUComponent, auto CreationFunction, auto DestructionFunction>
static void SetupGPUComponent(Engine::Core &core)
{
    auto &registry = core.GetRegistry();

    registry.on_construct<CPUComponent>().template connect<CreationFunction>(core);
    registry.on_destroy<CPUComponent>().template connect<DestructionFunction>(core);
    registry.on_destroy<GPUComponent>().template connect<DestructionFunction>(core);
}

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
    RegisterResource(Graphic::Resource::AmbientLight());

    SetupGPUComponent<Object::Component::Camera, Component::GPUCamera, &Graphic::System::OnCameraCreation,
                      &Graphic::System::OnCameraDestruction>(this->GetCore());
    SetupGPUComponent<Object::Component::Mesh, Component::GPUMesh, &Graphic::System::OnMeshCreation,
                      &Graphic::System::OnMeshDestruction>(this->GetCore());
    SetupGPUComponent<Object::Component::Transform, Component::GPUTransform, &Graphic::System::OnTransformCreation,
                      &Graphic::System::OnTransformDestruction>(this->GetCore());
    SetupGPUComponent<Object::Component::Material, Component::GPUMaterial, &Graphic::System::OnMaterialCreation,
                      &Graphic::System::OnMaterialDestruction>(this->GetCore());

    RegisterSystems<RenderingPipeline::Setup>(
        System::CreateInstance, System::CreateSurface, System::CreateAdapter, System::ReleaseInstance,
        System::RequestCapabilities, System::CreateDevice, System::CreateQueue, System::SetupQueue,
        System::ConfigureSurface, System::ReleaseAdapter, System::CreateEmptyTexture, System::CreateDefaultTexture,
        System::CreateDefaultSampler, System::CreateDefaultRenderPipeline, System::CreateDefaultMaterial,
        System::CreateAmbientLight);

    RegisterSystems<RenderingPipeline::Preparation>(System::PrepareEndRenderTexture, System::UpdateGPUTransforms,
                                                    System::UpdateGPUCameras, System::UpdateGPUMaterials);

    RegisterSystems<RenderingPipeline::CommandCreation>(System::ExecuteRenderPass);

    RegisterSystems<RenderingPipeline::Presentation>(System::Present);

    RegisterSystems<Engine::Scheduler::Shutdown>(System::ReleaseGPUBuffer, System::ReleaseBindingGroup,
                                                 System::ReleaseShader, System::ReleaseTexture, System::ReleaseSampler,
                                                 System::ReleaseContext);
}
