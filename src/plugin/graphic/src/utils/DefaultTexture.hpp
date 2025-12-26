#pragma once

#include <string_view>
#include <entt/core/hashed_string.hpp>

namespace Graphic::Utils {
static inline constexpr std::string_view DEFAULT_TEXTURE_NAME = "DEFAULT_TEXTURE";
static inline const entt::hashed_string DEFAULT_TEXTURE_ID{DEFAULT_TEXTURE_NAME.data(),
                                                            DEFAULT_TEXTURE_NAME.size()};
}
