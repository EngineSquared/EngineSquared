#pragma once

#include "utils/webgpu.hpp"

namespace Plugin::Graphic::Resource {
struct Surface {
    std::optional<wgpu::Surface> value;
    std::optional<wgpu::SurfaceCapabilities> capabilities;

    inline wgpu::Status getCapabilities(wgpu::Adapter &adapter, wgpu::SurfaceCapabilities &capabilities) const
    {
        return value->getCapabilities(adapter, &capabilities);
    }

    explicit Surface(std::optional<wgpu::Surface> surface = std::nullopt) : value(surface) {}
    ~Surface() = default;
};
} // namespace Plugin::Graphic::Resource
