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

    DeviceContext deviceContext;
};
} // namespace Graphic::Resource
