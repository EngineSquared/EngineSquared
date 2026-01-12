#pragma once

#include "core/Core.hpp"
#include <entt/core/hashed_string.hpp>

namespace Graphic::System {

constexpr entt::hashed_string END_RENDER_TEXTURE_ID{"end_render_texture"}; // TODO: put this in Utils
constexpr entt::hashed_string END_DEPTH_RENDER_TEXTURE_ID{"end_depth_render_texture"}; // TODO: remove this from Graphic plugin

void PrepareEndRenderTexture(Engine::Core &core);
} // namespace Graphic::System
