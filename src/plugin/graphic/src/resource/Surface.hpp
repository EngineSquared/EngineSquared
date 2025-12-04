#pragma once

#include "Logger.hpp"
#include "utils/webgpu.hpp"

namespace Graphic::Resource {
struct Surface {
    explicit Surface(std::optional<wgpu::Surface> surface = std::nullopt) : value(surface) {}
    ~Surface() = default;

    std::optional<wgpu::Surface> value;
    std::optional<wgpu::SurfaceCapabilities> capabilities;
    std::optional<wgpu::Texture> currentTexture;
    std::optional<wgpu::TextureView> currentTextureView;

    inline wgpu::Status updateCapabilities(wgpu::Adapter &adapter)
    {
        if (capabilities.has_value())
        {
            Log::Warn("Surface capabilities already requested, overwriting");
        }
        return value->getCapabilities(adapter, &(capabilities.value()));
    }

    inline void Release() noexcept
    {
        if (currentTextureView.has_value())
        {
            currentTextureView->release();
            currentTextureView.reset();
        }
        if (currentTexture.has_value())
        {
            currentTexture->release();
            currentTexture.reset();
        }
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
