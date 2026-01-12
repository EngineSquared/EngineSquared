#pragma once

#include <entt/core/hashed_string.hpp>

namespace DefaultPipeline::Component {
struct GPUMesh {
    using Id = entt::hashed_string;
    Id pointBufferId{};
    Id indexBufferId{};
};
}; // namespace DefaultPipeline::Component
