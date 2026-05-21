#pragma once

#include "utils/webgpu.hpp"

namespace Graphic::Resource {
class Adapter {
  public:
    explicit Adapter(wgpu::Adapter wgpuAdapter_) : wgpuAdapter(wgpuAdapter_) {}
    ~Adapter()
    {
        if (wgpuAdapter)
        {
            wgpuAdapter.release();
            wgpuAdapter = nullptr;
        }
    }

    Adapter(Adapter &other) = delete;
    Adapter &operator=(Adapter &other) = delete;

    // @note While using && ctor, you're destroying the previous adapter and will be invalid
    Adapter(Adapter &&other) noexcept : wgpuAdapter(other.wgpuAdapter)
    {
        other.wgpuAdapter = nullptr;
    }

    // @note While using && assignement, you're destroying the previous adapter and will be invalid
    Adapter &operator=(Adapter &&other) noexcept
    {
        if (this != &other)
        {
            if (wgpuAdapter)
            {
                wgpuAdapter.release();
            }
            wgpuAdapter = std::move(other.wgpuAdapter);
            other.wgpuAdapter = nullptr;
        }
        return *this;
    }

    wgpu::Adapter &operator*(void) { return wgpuAdapter; };
    wgpu::Adapter *operator->(void) { return &wgpuAdapter; };

  private:
    // @note We will assume that adapter is not null all the time
    wgpu::Adapter wgpuAdapter = nullptr;
};
} // namespace Graphic::Resource
