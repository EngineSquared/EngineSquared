#include "system/initialization/CreateDefaultTexture.hpp"
#include "resource/Context.hpp"
#include "resource/Texture.hpp"
#include "resource/TextureContainer.hpp"
#include "utils/DefaultTexture.hpp"

namespace Graphic::System {
void CreateDefaultTexture(Engine::Core &core)
{
    const Resource::Context &context = core.GetResource<Resource::Context>();
    Resource::TextureContainer &textureContainer = core.GetResource<Resource::TextureContainer>();

    Resource::Texture defaultTexture(context, Utils::DEFAULT_TEXTURE_NAME, glm::uvec2(1, 1),
                                     [](glm::uvec2) { return glm::u8vec4(255); });
    textureContainer.Add(Utils::DEFAULT_TEXTURE_ID, std::move(defaultTexture));
}
} // namespace Graphic::System
