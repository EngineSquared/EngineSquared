#pragma once

#include "resource/DeviceContext.hpp"
#include "resource/Surface.hpp"
#include "utils/webgpu.hpp"

namespace Plugin::Graphic::Resource {
class Context {
  public:
    Context() = default;
    ~Context() = default;

    void RequestDevice(void) { deviceContext.GetDevice() = adapter->requestDevice(deviceContext.GetDescriptor()); }

    std::optional<wgpu::Instance> instance;
    std::optional<Surface> surface;
    std::optional<wgpu::Adapter> adapter;
    DeviceContext deviceContext;
};
} // namespace Plugin::Graphic::Resource
