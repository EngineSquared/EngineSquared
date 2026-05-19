#pragma once

#include "utils/webgpu.hpp"

namespace Graphic::Resource {
class Instance {
  public:
    explicit Instance(wgpu::Instance wgpuInstance_) : wgpuInstance(wgpuInstance_) {}
    ~Instance()
    {
        if (wgpuInstance)
        {
            wgpuInstance.release();
            wgpuInstance = nullptr;
        }
    }

    Instance(Instance &other) = delete;
    Instance &operator=(Instance &other) = delete;

    // @note While using && ctor, you're destroying the previous Instance and will be invalid
    Instance(Instance &&other)
    {
        wgpuInstance = other.wgpuInstance;
        other.wgpuInstance = nullptr;
    }

    // @note While using && assignement, you're destroying the previous Instance and will be invalid
    Instance &operator=(Instance &&other)
    {
        wgpuInstance = other.wgpuInstance;
        other.wgpuInstance = nullptr;
        return *this;
    }

    wgpu::Instance &operator*(void) { return wgpuInstance; };
    wgpu::Instance *const operator->(void) { return &wgpuInstance; };

    const wgpu::Instance &operator*(void) const { return wgpuInstance; };
    const wgpu::Instance *const operator->(void) const { return &wgpuInstance; };

  private:
    // @note We will assume that Instance is not null all the time
    wgpu::Instance wgpuInstance = nullptr;
};
} // namespace Graphic::Resource
