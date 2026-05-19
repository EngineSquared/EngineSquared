#include "system/initialization/CreateDefaultTexture.hpp"
#include "resource/Context.hpp"
#include "resource/Texture.hpp"
#include "resource/TextureContainer.hpp"
#include "utils/DefaultTexture.hpp"

namespace Graphic::System {
void CreateDefaultTexture(Engine::Core &core)
{
    const auto &context = core.GetResource<Resource::Context>();
    auto &textureContainer = core.GetResource<Resource::TextureContainer>();
    const auto &queue = core.GetResource<Resource::Queue>();

    Resource::Texture defaultTexture(context, queue, Utils::DEFAULT_TEXTURE_NAME, glm::uvec2(1, 1),
                                     [](glm::uvec2) { return glm::u8vec4(255); });
    textureContainer.Add(Utils::DEFAULT_TEXTURE_ID, std::move(defaultTexture));
}
} // namespace Graphic::System
