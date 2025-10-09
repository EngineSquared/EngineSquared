#pragma once

#include "utils/webgpu.hpp"

namespace Plugin::Graphic::Resource {
struct Surface {
    std::optional<wgpu::Surface> value;

    Surface(std::optional<wgpu::Surface> surface = std::nullopt) : value(surface) {}
    ~Surface() = default;
};
} // namespace Plugin::Graphic::Resource
