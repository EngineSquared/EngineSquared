#include "system/initialization/CreateDefaultTexture.hpp"
#include "resource/DeviceContext.hpp"
#include "resource/Texture.hpp"
#include "resource/TextureContainer.hpp"
#include "utils/DefaultTexture.hpp"

namespace Graphic::System {
void CreateDefaultTexture(Engine::Core &core)
{
    const auto &deviceContext = core.GetResource<Resource::DeviceContext>();
    auto &textureContainer = core.GetResource<Resource::TextureContainer>();
    const auto &queue = core.GetResource<Resource::Queue>();

    Resource::Texture defaultTexture(deviceContext, queue, Utils::DEFAULT_TEXTURE_NAME, glm::uvec2(1, 1),
                                     [](glm::uvec2) { return glm::u8vec4(255); });
    textureContainer.Add(Utils::DEFAULT_TEXTURE_ID, std::move(defaultTexture));
}
} // namespace Graphic::System
