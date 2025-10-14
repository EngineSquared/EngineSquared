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

    void Release()
    {
        if (instance.has_value())
        {
            instance->release();
            instance.reset();
        }
        if (adapter.has_value())
        {
            adapter->release();
            adapter.reset();
        }
        if (queue.has_value())
        {
            queue->release();
            queue.reset();
        }
        deviceContext.Release();
        if (surface.has_value())
        {
            surface->Release();
            surface.reset();
        }
    }

    std::optional<wgpu::Instance> instance;
    std::optional<Surface> surface;
    std::optional<wgpu::Adapter> adapter;
    DeviceContext deviceContext;
    std::optional<wgpu::Queue> queue;
};
} // namespace Plugin::Graphic::Resource
