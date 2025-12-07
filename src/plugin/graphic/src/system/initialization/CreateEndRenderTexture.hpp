#pragma once

#include "core/Core.hpp"
#include <entt/core/hashed_string.hpp>

namespace Graphic::System {
constexpr entt::hashed_string END_RENDER_TEXTURE_ID{"end_render_texture"};

void CreateEndRenderTexture(Engine::Core &core);
} // namespace Graphic::System
