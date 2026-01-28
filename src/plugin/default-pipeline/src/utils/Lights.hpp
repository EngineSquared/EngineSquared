#pragma once

#include <entt/core/hashed_string.hpp>
#include <string_view>

namespace DefaultPipeline::Utils {

static inline constexpr std::string_view LIGHTS_BIND_GROUP_NAME = "LIGHTS_BIND_GROUP";
static inline const entt::hashed_string LIGHTS_BIND_GROUP_ID{LIGHTS_BIND_GROUP_NAME.data(),
                                                             LIGHTS_BIND_GROUP_NAME.size()};
} // namespace DefaultPipeline::Utils
