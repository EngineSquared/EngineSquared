#include "system/commandCreation/ExecuteRenderPass.hpp"
#include "component/Camera.hpp"
#include "entity/Entity.hpp"
#include "resource/RenderGraphContainer.hpp"

static Graphic::Resource::RenderGraph &GetRenderGraph(Engine::Core &core, const entt::hashed_string &name)
{
    auto &renderGraphContainer = core.GetResource<Graphic::Resource::RenderGraphContainer>();

    if (!renderGraphContainer.Contains(name))
    {
        throw std::runtime_error(fmt::format("RenderGraph '{}' not found in RenderGraphContainer.", name.data()));
    }

    return renderGraphContainer.Get(name);
}

void Graphic::System::ExecuteRenderPass(Engine::Core &core)
{
    auto &renderPassContainer = core.GetResource<Graphic::Resource::RenderGraphContainer>();

    core.GetRegistry().view<Object::Component::Camera>().each(
        [&renderPassContainer, &core](auto e, const Object::Component::Camera &camera) {
            renderPassContainer.GetOrDefault(entt::hashed_string{}).Execute(core);
        });
}
