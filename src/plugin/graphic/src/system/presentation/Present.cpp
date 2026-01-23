#include "system/presentation/Present.hpp"
#include "resource/Context.hpp"
#include "resource/TextureContainer.hpp"
#include "utils/EndRenderTexture.hpp"

void Graphic::System::Present(Engine::Core &core)
{
    auto &context = core.GetResource<Resource::Context>();

    if (!context.surface.has_value() || !context.surface->configured)
        return;

    context.surface->value->present();
    {
        auto &textureContainer = core.GetResource<Resource::TextureContainer>();
        textureContainer.Get(Utils::END_RENDER_TEXTURE_ID).TakeOwnership();
        textureContainer.Remove(Utils::END_RENDER_TEXTURE_ID);
    }
}
