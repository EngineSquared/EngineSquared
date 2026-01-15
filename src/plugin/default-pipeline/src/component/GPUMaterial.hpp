#pragma once

#include <entt/core/hashed_string.hpp>

namespace DefaultPipeline::Component {
struct GPUMaterial {
    using Id = entt::hashed_string;

    Id buffer{};
    Id texture{};
    Id sampler{};
    Id bindGroup{};
};
} // namespace DefaultPipeline::Component
