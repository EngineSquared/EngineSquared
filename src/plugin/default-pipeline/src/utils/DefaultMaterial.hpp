#pragma once

#include <entt/core/hashed_string.hpp>
#include <string_view>

namespace DefaultPipeline::Utils {
static inline constexpr std::string_view DEFAULT_MATERIAL_NAME = "DEFAULT_MATERIAL";
static inline const entt::hashed_string DEFAULT_MATERIAL_ID{DEFAULT_MATERIAL_NAME.data(), DEFAULT_MATERIAL_NAME.size()};
static inline constexpr std::string_view DEFAULT_MATERIAL_BIND_GROUP_NAME = "DEFAULT_MATERIAL_BIND_GROUP";
static inline const entt::hashed_string DEFAULT_MATERIAL_BIND_GROUP_ID{DEFAULT_MATERIAL_BIND_GROUP_NAME.data(),
                                                                       DEFAULT_MATERIAL_BIND_GROUP_NAME.size()};
} // namespace DefaultPipeline::Utils
