#pragma once

#include "component/Camera.hpp"
#include "component/Transform.hpp"
#include "glm/glm.hpp"
#include <entt/core/hashed_string.hpp>

namespace Graphic::Component {
struct GPUMaterial {
    using Id = entt::hashed_string;

    Id buffer{};
    Id bindGroup{};
};
}; // namespace Graphic::Component
