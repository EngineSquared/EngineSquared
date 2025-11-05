#pragma once

#include "utils/webgpu.hpp"

namespace Graphic::Resource {
struct Limits : wgpu::Limits {
  public:
    using wgpu::Limits::Limits;
    ~Limits() = default;

    Limits &setMaxBindGroups(auto value)
    {
        this->maxBindGroups = value;
        return *this;
    }
};
} // namespace Graphic::Resource
