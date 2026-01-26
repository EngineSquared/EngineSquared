#include "plugin/PluginDefaultPipeline.hpp"
#include "DefaultPipeline.hpp"
#include "RenderingPipeline.hpp"
#include "component/Material.hpp"
#include "plugin/PluginGraphic.hpp"

template <typename CPUComponent, typename GPUComponent, auto CreationFunction, auto DestructionFunction>
static void SetupGPUComponent(Engine::Core &core)
{
    auto &registry = core.GetRegistry();

    registry.on_construct<CPUComponent>().template connect<CreationFunction>(core);
    registry.on_destroy<CPUComponent>().template connect<DestructionFunction>(core);
    registry.on_destroy<GPUComponent>().template connect<DestructionFunction>(core);
}

void DefaultPipeline::Plugin::Bind()
{
    RequirePlugins<RenderingPipeline::Plugin, Graphic::Plugin>();

    RegisterResource(DefaultPipeline::Resource::AmbientLight());

    SetupGPUComponent<Object::Component::Camera, Component::GPUCamera, &System::OnCameraCreation,
                      &System::OnCameraDestruction>(this->GetCore());
    SetupGPUComponent<Object::Component::Mesh, Component::GPUMesh, &System::OnMeshCreation, &System::OnMeshDestruction>(
        this->GetCore());
    SetupGPUComponent<Object::Component::Transform, Component::GPUTransform, &System::OnTransformCreation,
                      &System::OnTransformDestruction>(this->GetCore());
    SetupGPUComponent<Object::Component::Material, Component::GPUMaterial, &System::OnMaterialCreation,
                      &System::OnMaterialDestruction>(this->GetCore());
    SetupGPUComponent<Object::Component::DirectionalLight, Component::GPUDirectionalLight,
                      &System::OnDirectionalLightCreation, &System::OnDirectionalLightDestruction>(this->GetCore());

    RegisterSystems<RenderingPipeline::Setup>(System::Create3DGraph, System::CreateDefaultMaterial,
                                              System::CreateAmbientLight, System::CreatePointLights, System::CreateDirectionalLights, System::CreateLights);

    RegisterSystems<RenderingPipeline::Preparation>(
        System::UpdateGPUTransforms, System::UpdateGPUCameras, System::UpdateGPUMaterials, System::UpdateGPUMeshes,
        System::UpdateGPUDirectionalLight, System::UpdateAmbientLight, System::UpdatePointLights);
}
