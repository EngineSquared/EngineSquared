#pragma once

#include "utils/webgpu.hpp"

namespace Plugin::Graphic::Resource {
struct Limits : wgpu::Limits {
  public:
    Limits(void) = default;
    ~Limits() = default;

    Limits &setMaxBindGroups(auto value)
    {
        this->maxBindGroups = value;
        return *this;
    }
};
} // namespace Plugin::Graphic::Resource
