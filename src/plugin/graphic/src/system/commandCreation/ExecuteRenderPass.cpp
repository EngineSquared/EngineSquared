#include "system/commandCreation/ExecuteRenderPass.hpp"
#include "component/Camera.hpp"
#include "entity/Entity.hpp"
#include "exception/MissingRenderGraphError.hpp"
#include "resource/RenderGraphContainer.hpp"

void Graphic::System::ExecuteRenderPass(Engine::Core &core)
{
    auto &renderPassContainer = core.GetResource<Graphic::Resource::RenderGraphContainer>();

    core.GetRegistry().view<Object::Component::Camera>().each(
        [&renderPassContainer, &core](auto e, const Object::Component::Camera &camera) {
            renderPassContainer.GetOrDefault(entt::hashed_string{}).Execute(core);
        });
}
