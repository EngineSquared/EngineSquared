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

/**
 * @brief Configure and register the default rendering pipeline for the plugin.
 *
 * Registers required plugin dependencies and runtime resources, wires CPU component
 * lifecycle events to GPU creation/destruction handlers for camera, mesh, transform,
 * and material components, and registers the rendering setup and preparation systems.
 *
 * The setup systems registered include: CreateDefaultRenderPipeline, Create3DGraph,
 * CreateDefaultMaterial, CreateAmbientLight, and CreatePointLights.
 *
 * The preparation systems registered include: UpdateGPUTransforms, UpdateGPUCameras,
 * UpdateGPUMaterials, UpdateGPUMeshes, UpdateAmbientLight, and UpdatePointLights.
 */
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

    RegisterSystems<RenderingPipeline::Setup>(System::CreateDefaultRenderPipeline, System::Create3DGraph,
                                              System::CreateDefaultMaterial, System::CreateAmbientLight,
                                              System::CreatePointLights);

    RegisterSystems<RenderingPipeline::Preparation>(System::UpdateGPUTransforms, System::UpdateGPUCameras,
                                                    System::UpdateGPUMaterials, System::UpdateGPUMeshes,
                                                    System::UpdateAmbientLight, System::UpdatePointLights);
}
