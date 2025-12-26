#include "system/initialization/CreateEmptyTexture.hpp"
#include "resource/Context.hpp"
#include "resource/Texture.hpp"
#include "resource/TextureContainer.hpp"
#include "utils/EmptyTexture.hpp"

namespace Graphic::System {
void CreateEmptyTexture(Engine::Core &core)
{
    const Resource::Context &context = core.GetResource<Resource::Context>();
    Resource::TextureContainer &textureContainer = core.GetResource<Resource::TextureContainer>();

    Resource::Texture emptyTexture(context, Utils::EMPTY_TEXTURE_NAME, glm::uvec2(2, 2), [](glm::uvec2 pos) {
        glm::u8vec4 color;
        color.r = ((pos.x + pos.y) % 2 == 0) ? 255 : 0;
        color.g = 0;
        color.b = ((pos.x + pos.y) % 2 == 0) ? 255 : 0;
        color.a = 255;
        return color;
    });

    textureContainer.SetDefault(std::move(emptyTexture));
}
}
