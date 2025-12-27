#pragma once

#include <entt/core/hashed_string.hpp>
#include <string_view>

namespace Graphic::Utils {
static inline constexpr std::string_view EMPTY_TEXTURE_NAME = "EMPTY_TEXTURE";
static inline const entt::hashed_string EMPTY_TEXTURE_ID{EMPTY_TEXTURE_NAME.data(), EMPTY_TEXTURE_NAME.size()};
} // namespace Graphic::Utils
