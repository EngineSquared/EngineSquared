#pragma once

#include <entt/core/hashed_string.hpp>
#include <string_view>

namespace Graphic::Utils {
static inline constexpr std::string_view DEFAULT_TEXTURE_NAME = "DEFAULT_TEXTURE";
static inline const entt::hashed_string DEFAULT_TEXTURE_ID{DEFAULT_TEXTURE_NAME.data(), DEFAULT_TEXTURE_NAME.size()};
} // namespace Graphic::Utils
