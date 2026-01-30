#pragma once

#include <entt/core/hashed_string.hpp>
#include <string_view>

namespace DefaultPipeline::Utils {

static inline constexpr size_t MAX_DIRECTIONAL_LIGHTS = 64;

static inline constexpr std::string_view DIRECTIONAL_LIGHTS_BUFFER_NAME = "DIRECTIONAL_LIGHTS_BUFFER";
static inline const entt::hashed_string DIRECTIONAL_LIGHTS_BUFFER_ID{DIRECTIONAL_LIGHTS_BUFFER_NAME.data(),
                                                                     DIRECTIONAL_LIGHTS_BUFFER_NAME.size()};

static inline constexpr std::string_view DIRECTIONAL_LIGHTS_SHADOW_TEXTURE_NAME = "DIRECTIONAL_LIGHTS_SHADOW_TEXTURE";
static inline const entt::hashed_string DIRECTIONAL_LIGHTS_SHADOW_TEXTURE_ID{
    DIRECTIONAL_LIGHTS_SHADOW_TEXTURE_NAME.data(), DIRECTIONAL_LIGHTS_SHADOW_TEXTURE_NAME.size()};

static inline constexpr std::string_view DIRECTIONAL_LIGHTS_SHADOW_SAMPLER_NAME = "DIRECTIONAL_LIGHTS_SHADOW_SAMPLER";
static inline const entt::hashed_string DIRECTIONAL_LIGHTS_SHADOW_SAMPLER_ID{
    DIRECTIONAL_LIGHTS_SHADOW_SAMPLER_NAME.data(), DIRECTIONAL_LIGHTS_SHADOW_SAMPLER_NAME.size()};

static inline constexpr std::string_view DIRECTIONAL_LIGHTS_SHADOW_BINDGROUP_NAME =
    "DIRECTIONAL_LIGHTS_SHADOW_BINDGROUP";
static inline const entt::hashed_string DIRECTIONAL_LIGHTS_SHADOW_BINDGROUP_ID{
    DIRECTIONAL_LIGHTS_SHADOW_BINDGROUP_NAME.data(), DIRECTIONAL_LIGHTS_SHADOW_BINDGROUP_NAME.size()};

} // namespace DefaultPipeline::Utils
