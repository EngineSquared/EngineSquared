#pragma once

#include <entt/core/hashed_string.hpp>
#include <string_view>

namespace Graphic::Utils {
static inline constexpr std::string_view DEFAULT_SAMPLER_NAME = "DEFAULT_SAMPLER";
static inline const entt::hashed_string DEFAULT_SAMPLER_ID{DEFAULT_SAMPLER_NAME.data(), DEFAULT_SAMPLER_NAME.size()};
} // namespace Graphic::Utils
