#include "system/initialization/CreateEmptyTexture.hpp"
#include "resource/DeviceContext.hpp"
#include "resource/Texture.hpp"
#include "resource/TextureContainer.hpp"
#include "utils/EmptyTexture.hpp"

namespace Graphic::System {
void CreateEmptyTexture(Engine::Core &core)
{
    const auto &deviceContext = core.GetResource<Resource::DeviceContext>();
    const auto &queue = core.GetResource<Resource::Queue>();
    Resource::TextureContainer &textureContainer = core.GetResource<Resource::TextureContainer>();

    Resource::Texture emptyTexture(deviceContext, queue, Utils::EMPTY_TEXTURE_NAME, glm::uvec2(2, 2),
                                   [](glm::uvec2 pos) {
                                       glm::u8vec4 color;
                                       color.r = ((pos.x + pos.y) % 2 == 0) ? 255 : 0;
                                       color.g = 0;
                                       color.b = ((pos.x + pos.y) % 2 == 0) ? 255 : 0;
                                       color.a = 255;
                                       return color;
                                   });

    textureContainer.SetDefault(std::move(emptyTexture));
}
} // namespace Graphic::System
