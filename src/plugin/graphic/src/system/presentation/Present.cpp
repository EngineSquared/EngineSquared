#include "system/presentation/Present.hpp"
#include "resource/Context.hpp"
#include "resource/TextureContainer.hpp"
#include "utils/EndRenderTexture.hpp"

void Graphic::System::Present(Engine::Core &core)
{
    auto &context = core.GetResource<Resource::Context>();

    if (!core.HasResource<Resource::Surface>())
    {
        return;
    }
    auto &surface = core.GetResource<Resource::Surface>();

    if (!surface.value.has_value() || !surface.configured)
        return;

    surface.value->present();
    {
        auto &textureContainer = core.GetResource<Resource::TextureContainer>();
        if (textureContainer.Contains(Utils::END_RENDER_TEXTURE_ID))
        {
            textureContainer.Get(Utils::END_RENDER_TEXTURE_ID).TakeOwnership();
            textureContainer.Remove(Utils::END_RENDER_TEXTURE_ID);
        }
    }
}
