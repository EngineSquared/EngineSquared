#pragma once

#include "utils/ABindGroupLayoutEntry.hpp"
#include "utils/webgpu.hpp"

namespace Plugin::Graphic::Utils { // TODO: put this file in the correct forder and update its namespace
class SamplerBindGroupLayoutEntry : public ABindGroupLayoutEntry {
  public:
    SamplerBindGroupLayoutEntry(const std::string &name) : ABindGroupLayoutEntry(name) {}
    ~SamplerBindGroupLayoutEntry() = default;

    virtual std::vector<ValidationError> validate(void) const override
    {
        std::vector<ValidationError> errors = ABindGroupLayoutEntry::validate();
        if (!this->_isSamplerTypeSet)
        {
            errors.push_back({"Sampler type is not set", fmt::format("SamplerBindGroupLayoutEntry({})", this->name),
                              ValidationError::Severity::Error});
        }
        return errors;
    }

    inline SamplerBindGroupLayoutEntry &setSamplerType(const wgpu::SamplerBindingType &type)
    {
        this->entry.sampler.type = type;
        this->_isSamplerTypeSet = true;
        return *this;
    }

  private:
    bool _isSamplerTypeSet = false;
};
} // namespace Plugin::Graphic::Utils
