#pragma once

#include "component/AmbientLight.hpp"
#include <entt/core/hashed_string.hpp>
#include <string_view>

namespace Graphic::Utils {
static inline const Object::Component::AmbientLight DEFAULT_AMBIENT_LIGHT{
    glm::vec3{1.0f, 1.0f, 1.0f}
};
static inline constexpr std::string_view AMBIENT_LIGHT_BUFFER_NAME = "AMBIENT_LIGHT_BUFFER";
static inline const entt::hashed_string AMBIENT_LIGHT_BUFFER_ID{AMBIENT_LIGHT_BUFFER_NAME.data(),
                                                                AMBIENT_LIGHT_BUFFER_NAME.size()};
static inline constexpr std::string_view AMBIENT_LIGHT_BIND_GROUP_NAME = "AMBIENT_LIGHT_BIND_GROUP";
static inline const entt::hashed_string AMBIENT_LIGHT_BIND_GROUP_ID{AMBIENT_LIGHT_BIND_GROUP_NAME.data(),
                                                                    AMBIENT_LIGHT_BIND_GROUP_NAME.size()};
} // namespace Graphic::Utils
