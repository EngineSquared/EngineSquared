#pragma once

#include "component/Camera.hpp"
#include "component/Transform.hpp"
#include "glm/glm.hpp"
#include <entt/core/hashed_string.hpp>

namespace DefaultPipeline::Component {
struct GPUMaterial {
    using Id = entt::hashed_string;

    Id buffer{};
    Id texture{};
    Id sampler{};
    Id bindGroup{};
};
}; // namespace DefaultPipeline::Component
