#include "system/commandCreation/ExecuteRenderPass.hpp"
#include "component/Camera.hpp"
#include "resource/RenderGraphContainer.hpp"
#include "entity/Entity.hpp"

void Graphic::System::ExecuteRenderPass(Engine::Core &core)
{
    auto &renderPassContainer = core.GetResource<Graphic::Resource::RenderGraphContainer>();

    core.GetRegistry().view<Object::Component::Camera>().each(
        [&renderPassContainer, &core](auto e, const Object::Component::Camera &camera) {
            if (!renderPassContainer.Contains(camera.pipeline)) {
                Engine::Entity entity(e);
                Log::Error(fmt::format(
                    "ExecuteRenderPass: Camera with Pipeline '{}' not found for entity {} camera component.",
                    camera.pipeline.data() ? camera.pipeline.data() : "null",
                    Log::EntityToDebugString(static_cast<Engine::Entity::entity_id_type>(entity))));
                return;
            }
            renderPassContainer.Get(camera.pipeline).Execute(core);
        });
}
