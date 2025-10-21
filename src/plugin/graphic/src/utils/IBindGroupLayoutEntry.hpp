#pragma once

#include "utils/IValidable.hpp"
#include "utils/webgpu.hpp"

namespace Plugin::Graphic::Utils { // TODO: put this file in the correct forder and update its namespace

class IBindGroupLayoutEntry : public virtual IValidable {
  public:
    virtual const wgpu::BindGroupLayoutEntry &getEntry() const = 0;
    virtual const std::string &getName() const = 0;
};
} // namespace Plugin::Graphic::Utils
