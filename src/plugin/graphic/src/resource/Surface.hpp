#pragma once

#include "Logger.hpp"
#include "utils/webgpu.hpp"

namespace Graphic::Resource {
struct Surface {
    explicit Surface(std::optional<wgpu::Surface> surface = std::nullopt) : value(surface) {}
    ~Surface() = default;

    std::optional<wgpu::Surface> value;
    std::optional<wgpu::SurfaceCapabilities> capabilities;

    inline wgpu::Status updateCapabilities(wgpu::Adapter &adapter)
    {
        if (capabilities.has_value())
        {
            Log::Warn("Surface capabilities already requested, overwriting");
        }
        wgpu::SurfaceCapabilities caps;
        wgpu::Status status = value->getCapabilities(adapter, &caps);
        capabilities = caps;
        return status;
    }

    inline void Release() noexcept
    {
        if (value.has_value())
        {
            value->release();
            value.reset();
        }
        if (capabilities.has_value())
        {
            capabilities.reset();
        }
    }
};
} // namespace Graphic::Resource
