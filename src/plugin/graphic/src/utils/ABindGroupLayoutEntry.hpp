#pragma once

#include "utils/webgpu.hpp"

namespace Plugin::Graphic::Utils { // TODO: put this file in the correct forder and update its namespace

class ABindGroupLayoutEntry {
  public:
    std::string name;

    explicit ABindGroupLayoutEntry(const std::string &name) : name(name) {}
    virtual ~ABindGroupLayoutEntry() = default;

    inline const wgpu::BindGroupLayoutEntry &getEntry() const { return this->entry; }

    inline ABindGroupLayoutEntry &setBinding(uint32_t binding)
    {
        this->entry.binding = binding;
        this->isBindingSet = true;
        return *this;
    }

    inline ABindGroupLayoutEntry &setVisibility(wgpu::ShaderStage visibility)
    {
        this->entry.visibility = visibility;
        this->isVisibilitySet = true;
        return *this;
    }

    virtual bool isComplete() const { return this->isBindingSet && this->isVisibilitySet; }

  protected:
    wgpu::BindGroupLayoutEntry entry;
    bool isBindingSet = false;
    bool isVisibilitySet = false;
};
} // namespace Plugin::Graphic::Utils
