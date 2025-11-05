#pragma once

#include "utils/webgpu.hpp"

namespace Graphic::Resource {
struct DeviceContext {
  public:
    DeviceContext(void) = default;
    ~DeviceContext() = default;

    auto &GetDescriptor() { return _descriptor; }
    auto &GetDevice() { return _device; }

    void Release() noexcept
    {
        if (_device.has_value())
        {
            _device->release();
            _device.reset();
        }
    }

  private:
    wgpu::DeviceDescriptor _descriptor = wgpu::DeviceDescriptor(wgpu::Default);
    std::optional<wgpu::Device> _device = std::nullopt;
};
} // namespace Graphic::Resource
