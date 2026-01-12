#pragma once

#include <entt/core/hashed_string.hpp>

namespace DefaultPipeline::Component {
struct GPUTransform {
    using Id = entt::hashed_string;
    Id modelMatrixBuffer{};
    Id bindGroup{};
};
}; // namespace DefaultPipeline::Component
