#pragma once

#include <entt/core/hashed_string.hpp>
#include <string_view>

namespace DefaultPipeline::Utils {

static inline constexpr size_t MAX_POINT_LIGHTS = 64;

static inline constexpr std::string_view POINT_LIGHTS_BUFFER_NAME = "POINT_LIGHTS_BUFFER";
static inline const entt::hashed_string POINT_LIGHTS_BUFFER_ID{POINT_LIGHTS_BUFFER_NAME.data(),
                                                               POINT_LIGHTS_BUFFER_NAME.size()};
} // namespace DefaultPipeline::Utils
