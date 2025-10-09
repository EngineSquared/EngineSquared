#pragma once

#include "utils/webgpu.hpp"
#include "Surface.hpp"

namespace Plugin::Graphic::Resource {
class Context {
  public:
    Context() = default;
    ~Context() = default;

    std::optional<wgpu::Instance> instance;
    std::optional<Surface> surface;
};
} // namespace Plugin::Graphic::Resource
