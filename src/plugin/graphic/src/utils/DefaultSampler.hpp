#pragma once

#include <string_view>
#include <entt/core/hashed_string.hpp>

namespace Graphic::Utils {
static inline constexpr std::string_view DEFAULT_SAMPLER_NAME = "DEFAULT_SAMPLER";
static inline const entt::hashed_string DEFAULT_SAMPLER_ID{DEFAULT_SAMPLER_NAME.data(),
                                                            DEFAULT_SAMPLER_NAME.size()};
}
