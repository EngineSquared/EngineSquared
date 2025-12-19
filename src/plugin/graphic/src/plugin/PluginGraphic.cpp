#include "plugin/PluginGraphic.hpp"
#include "Graphic.hpp"
#include "RenderingPipeline.hpp"
#include "plugin/PluginWindow.hpp"
#include "scheduler/Shutdown.hpp"
#include "component/Camera.hpp"

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

    {
        struct OnCameraCreationBinder {
            void CallFunction(entt::registry &registry, entt::entity entity) {
                Graphic::System::OnCameraCreation(core, entity);
            }
            Engine::Core &core;
        };
        static OnCameraCreationBinder binder{this->GetCore()};
        this->GetCore().GetRegistry().on_construct<Object::Component::Camera>()
            .connect<&OnCameraCreationBinder::CallFunction>(binder);
    }

    RegisterSystems<RenderingPipeline::Setup>(
        System::CreateInstance, System::CreateSurface, System::CreateAdapter, System::ReleaseInstance,
        System::RequestCapabilities, System::CreateDevice, System::CreateQueue, System::SetupQueue,
        System::ConfigureSurface, System::ReleaseAdapter, System::CreateDefaultRenderPipeline);

    RegisterSystems<RenderingPipeline::Preparation>(System::CreateEndRenderTexture);

    RegisterSystems<RenderingPipeline::CommandCreation>(System::ExecuteRenderPass);

    RegisterSystems<RenderingPipeline::Presentation>(System::Present);

    RegisterSystems<Engine::Scheduler::Shutdown>(System::ReleaseGPUBuffer, System::ReleaseBindingGroup,
                                                 System::ReleaseShader, System::ReleaseTexture, System::ReleaseSampler,
                                                 System::ReleaseContext);
}
