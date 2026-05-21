#pragma once

#include "resource/Adapter.hpp"
#include "resource/DeviceContext.hpp"
#include "resource/Surface.hpp"
#include "utils/webgpu.hpp"

namespace Graphic::Resource {
class Context {
  public:
    Context() = default;
    ~Context() = default;

    void RequestDevice(Adapter &adapter)
    {
        deviceContext.GetDevice() = adapter->requestDevice(deviceContext.GetDescriptor());
    }

    // void Release()
    // {
    //     deviceContext.Release();
    // }

    DeviceContext deviceContext;
};
} // namespace Graphic::Resource
