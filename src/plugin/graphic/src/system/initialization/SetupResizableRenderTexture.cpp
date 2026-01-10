#include "system/initialization/SetupResizableRenderTexture.hpp"

#include "event/OnResize.hpp"
#include "resource/EventManager.hpp"
#include "system/initialization/ConfigureSurface.hpp"

namespace Graphic::System {

static void OnWindowResize(Engine::Core &core, const glm::ivec2 &newSize) { ConfigureSurface(core); }

void SetupResizableRenderTexture(Engine::Core &core)
{
    auto &eventManager = core.GetResource<Event::Resource::EventManager>();

    eventManager.RegisterCallback<Window::Event::OnResize>(
        [&core](const Window::Event::OnResize &event) { OnWindowResize(core, event.newSize); });
}

} // namespace Graphic::System
