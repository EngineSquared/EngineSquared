#include "system/presentation/Present.hpp"
#include "resource/Context.hpp"
#include "resource/TextureContainer.hpp"

void Graphic::System::Present(Engine::Core &core)
{
    auto &context = core.GetResource<Graphic::Resource::Context>();

    if (!context.surface.has_value() || !context.surface->configured)
        return;

    context.surface->value->present();
}
