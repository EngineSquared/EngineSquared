#pragma once

#include <string_view>
#include <entt/core/hashed_string.hpp>

namespace Graphic::Utils {
static inline constexpr std::string_view DEFAULT_MATERIAL_NAME = "DEFAULT_MATERIAL";
static inline const entt::hashed_string DEFAULT_MATERIAL_ID{DEFAULT_MATERIAL_NAME.data(),
                                                            DEFAULT_MATERIAL_NAME.size()};
}
