#pragma once

#include "utils/webgpu.hpp"
#include "utils/IValidable.hpp"

namespace Plugin::Graphic::Utils { // TODO: put this file in the correct forder and update its namespace

class ABindGroupLayoutEntry : public virtual IValidable {
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

    std::vector<ValidationError> validate(void) const override
    {
        std::vector<ValidationError> errors;
        if (!this->isBindingSet)
        {
            errors.push_back({ "Binding is not set", "BindGroupLayoutEntry: " + this->name, ValidationError::Severity::Error });
        }
        if (!this->isVisibilitySet)
        {
            errors.push_back({ "Visibility is not set", "BindGroupLayoutEntry: " + this->name, ValidationError::Severity::Error });
        }
        return errors;
    }

  protected:
    wgpu::BindGroupLayoutEntry entry;
    bool isBindingSet = false;
    bool isVisibilitySet = false;
};
} // namespace Plugin::Graphic::Utils
