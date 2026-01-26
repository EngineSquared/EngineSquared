#pragma once

#include <entt/core/hashed_string.hpp>
#include <string_view>

namespace DefaultPipeline::Utils {

static inline constexpr size_t MAX_DIRECTIONAL_LIGHTS = 64;

static inline constexpr std::string_view DIRECTIONAL_LIGHTS_BUFFER_NAME = "DIRECTIONAL_LIGHTS_BUFFER";
static inline const entt::hashed_string DIRECTIONAL_LIGHTS_BUFFER_ID{DIRECTIONAL_LIGHTS_BUFFER_NAME.data(),
                                                               DIRECTIONAL_LIGHTS_BUFFER_NAME.size()};
} // namespace DefaultPipeline::Utils
