#pragma once

#include "Logger.hpp"
#include "utils/webgpu.hpp"

namespace Plugin::Graphic::Resource {
struct Surface {
    std::optional<wgpu::Surface> value;
    std::optional<wgpu::SurfaceCapabilities> capabilities;

    inline wgpu::Status updateCapabilities(wgpu::Adapter &adapter)
    {
        if (capabilities.has_value())
        {
            Log::Warn("Surface capabilities already requested, overwriting");
        }
        return value->getCapabilities(adapter, &(capabilities.value()));
    }

    explicit Surface(std::optional<wgpu::Surface> surface = std::nullopt) : value(surface) {}
    ~Surface() = default;
};
} // namespace Plugin::Graphic::Resource
