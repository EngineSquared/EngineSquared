#pragma once

#include "exception/UnsupportedTextureFormatError.hpp"
#include "resource/Context.hpp"
#include "resource/Image.hpp"
#include "utils/GetBytesPerPixel.hpp"
#include "utils/webgpu.hpp"
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <functional>
#include <glm/gtc/packing.hpp>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <vector>

namespace Graphic::Resource {

class TextureView {
  public:
    TextureView(void) = default;
    TextureView(wgpu::TextureView &&textureView) : _webgpuView(textureView) {}

    ~TextureView() { Delete(); }
    TextureView(const TextureView &) = delete;
    TextureView &operator=(const TextureView &) = delete;

    TextureView(TextureView &&other) noexcept : _webgpuView(std::move(other._webgpuView))
    {
        other._webgpuView = nullptr;
    }

    TextureView &operator=(TextureView &&other) noexcept
    {
        if (this != &other)
        {
            if (_webgpuView != nullptr)
            {
                _webgpuView.release();
                _webgpuView = nullptr;
            }
            _webgpuView = std::move(other._webgpuView);
            other._webgpuView = nullptr;
        }
        return *this;
    }

    operator wgpu::TextureView() const { return _webgpuView; }

    const wgpu::TextureView &GetWebGPUView() const { return _webgpuView; }

    void Delete()
    {
        if (_webgpuView != nullptr)
        {
            _webgpuView.release();
            _webgpuView = nullptr;
        }
    }

  private:
    wgpu::TextureView _webgpuView = nullptr;
};
} // namespace Graphic::Resource
