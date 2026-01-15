#pragma once

#include "core/Core.hpp"
#include <entt/core/hashed_string.hpp>

namespace Graphic::System {

constexpr entt::hashed_string END_DEPTH_RENDER_TEXTURE_ID{
    "end_depth_render_texture"}; // TODO: remove this from Graphic plugin, as we don't theorically need it here

void PrepareEndRenderTexture(Engine::Core &core);
} // namespace Graphic::System
