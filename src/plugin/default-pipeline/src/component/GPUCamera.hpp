#pragma once

#include "Logger.hpp"
#include "component/Camera.hpp"
#include "component/Transform.hpp"
#include "glm/glm.hpp"
#include <entt/core/hashed_string.hpp>

namespace DefaultPipeline::Component {
struct GPUCamera {
    using Id = entt::hashed_string;

    Id buffer{};
    Id bindGroup{};
    Id pipeline{};
    Id targetTexture{};
};
}; // namespace DefaultPipeline::Component
