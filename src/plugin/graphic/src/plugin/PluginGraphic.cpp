#include "plugin/PluginGraphic.hpp"
#include "Graphic.hpp"
#include "RenderingPipeline.hpp"
#include "component/Camera.hpp"
#include "component/Mesh.hpp"
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

    // TODO: Put those into dedicated systems
    { // GPUCamera
        struct OnCameraCreationBinder {
            void CallFunction(entt::registry &registry, entt::entity entity)
            {
                Graphic::System::OnCameraCreation(core, entity);
            }
            Engine::Core &core;
        };
        static OnCameraCreationBinder binder{this->GetCore()};
        this->GetCore()
            .GetRegistry()
            .on_construct<Object::Component::Camera>()
            .connect<&OnCameraCreationBinder::CallFunction>(binder);
    }
    { // GPUMesh
        struct OnMeshCreationBinder {
            void CallFunction(entt::registry &registry, entt::entity e)
            {
                Engine::Entity entity{e};
                Graphic::Component::GPUMesh &GPUMesh = entity.AddComponent<Graphic::Component::GPUMesh>(core);
                const auto &mesh = entity.GetComponents<Object::Component::Mesh>(core);

                // TODO: use name if exists
                std::string entityString =
                    Log::EntityToDebugString(static_cast<Engine::Entity::entity_id_type>(entity));

                auto &gpuBufferContainer = core.GetResource<Graphic::Resource::GPUBufferContainer>();

                std::string pointBufferName = "POINT_BUFFER_" + entityString;
                entt::hashed_string pointBufferUUID{pointBufferName.data(), pointBufferName.size()};
                gpuBufferContainer.Add(pointBufferUUID, std::make_unique<Graphic::Resource::PointGPUBuffer>(entity));
                gpuBufferContainer.Get(pointBufferUUID)->Create(core);
                GPUMesh.pointBufferId = pointBufferUUID;

                std::string indexBufferName = "INDEX_BUFFER_" + entityString;
                entt::hashed_string indexBufferUUID{indexBufferName.data(), indexBufferName.size()};
                gpuBufferContainer.Add(indexBufferUUID, std::make_unique<Graphic::Resource::IndexGPUBuffer>(entity));
                gpuBufferContainer.Get(indexBufferUUID)->Create(core);
                GPUMesh.indexBufferId = indexBufferUUID;
            }
            Engine::Core &core;
        };
        static OnMeshCreationBinder binder{this->GetCore()};
        this->GetCore()
            .GetRegistry()
            .on_construct<Object::Component::Mesh>()
            .connect<&OnMeshCreationBinder::CallFunction>(binder);
    }
    { // GPUTransform
        struct OnTransformCreationBinder {
            void CallFunction(entt::registry &registry, entt::entity e)
            {
                Engine::Entity entity{e};
                Graphic::Component::GPUTransform &GPUTransform =
                    entity.AddComponent<Graphic::Component::GPUTransform>(core);
                const auto &transform = entity.GetComponents<Object::Component::Transform>(core);

                std::string entityString =
                    Log::EntityToDebugString(static_cast<Engine::Entity::entity_id_type>(entity));

                auto &gpuBufferContainer = core.GetResource<Graphic::Resource::GPUBufferContainer>();

                std::string transformBufferName = "TRANSFORM_BUFFER_" + entityString;
                entt::hashed_string transformBufferId{transformBufferName.data(), transformBufferName.size()};
                gpuBufferContainer.Add(transformBufferId,
                                       std::make_unique<Graphic::Resource::TransformGPUBuffer>(entity));
                gpuBufferContainer.Get(transformBufferId)->Create(core);
                GPUTransform.modelMatrixBuffer = transformBufferId;
                auto &bindGroupManager = core.GetResource<Graphic::Resource::BindGroupManager>();
                std::string bindGroupName = "TRANSFORM_BIND_GROUP_" + entityString;
                entt::hashed_string bindGroupId{bindGroupName.data(), bindGroupName.size()};
                Resource::BindGroup bindGroup(core, "DEFAULT_RENDER_PASS_SHADER", 1,
                                              {
                                                  {
                                                   0, Resource::BindGroup::Asset::Type::Buffer,
                                                   transformBufferId, gpuBufferContainer.Get(transformBufferId)->GetBuffer().getSize(),
                                                   },
                });
                bindGroupManager.Add(bindGroupId, std::move(bindGroup));
                GPUTransform.bindGroup = bindGroupId;
            }
            Engine::Core &core;
        };
        static OnTransformCreationBinder binder{this->GetCore()};
        this->GetCore()
            .GetRegistry()
            .on_construct<Object::Component::Transform>()
            .connect<&OnTransformCreationBinder::CallFunction>(binder);
    }

    RegisterSystems<RenderingPipeline::Setup>(
        System::CreateInstance, System::CreateSurface, System::CreateAdapter, System::ReleaseInstance,
        System::RequestCapabilities, System::CreateDevice, System::CreateQueue, System::SetupQueue,
        System::ConfigureSurface, System::ReleaseAdapter, System::CreateDefaultRenderPipeline);

    RegisterSystems<RenderingPipeline::Preparation>(System::CreateEndRenderTexture, [](Engine::Core &core) {
        core.GetRegistry().view<Object::Component::Camera, Graphic::Component::GPUCamera>().each(
            [&](auto e, const Object::Component::Camera &camera, Graphic::Component::GPUCamera &gpuCamera) {
                Engine::Entity entity{e};
                auto &bindGroupManager = core.GetResource<Graphic::Resource::BindGroupManager>();
                bindGroupManager.Remove(gpuCamera.bindGroup);
                auto &gpuBufferContainer = core.GetResource<Graphic::Resource::GPUBufferContainer>();
                gpuBufferContainer.Remove(gpuCamera.buffer);
                entity.RemoveComponent<Graphic::Component::GPUCamera>(core);
                Graphic::System::OnCameraCreation(core, entity);
            });
    });

    RegisterSystems<RenderingPipeline::CommandCreation>(System::ExecuteRenderPass);

    RegisterSystems<RenderingPipeline::Presentation>(System::Present);

    RegisterSystems<Engine::Scheduler::Shutdown>(System::ReleaseGPUBuffer, System::ReleaseBindingGroup,
                                                 System::ReleaseShader, System::ReleaseTexture, System::ReleaseSampler,
                                                 System::ReleaseContext);
}
