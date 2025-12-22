#pragma once

#include "glm/glm.hpp"
#include <entt/core/hashed_string.hpp>

namespace Graphic::Component {
struct GPUCamera {
    using Id = entt::hashed_string;
    // TODO: find a way to update this on window resize
    float aspectRatio = 800.0f / 800.0f;
    glm::mat4 projection;
    glm::mat4 view;
    glm::mat4 viewProjection;
    glm::mat4 inverseViewProjection;
    Id buffer{};
    Id bindGroup{};
    Id pipeline{};
};
}; // namespace Graphic::Component
