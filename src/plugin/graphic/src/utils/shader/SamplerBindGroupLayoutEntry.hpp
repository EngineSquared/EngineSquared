#pragma once

#include "utils/shader/ABindGroupLayoutEntry.hpp"
#include "utils/webgpu.hpp"

namespace Graphic::Utils {
class SamplerBindGroupLayoutEntry : public ABindGroupLayoutEntry<SamplerBindGroupLayoutEntry> {
  public:
    SamplerBindGroupLayoutEntry(const std::string &name) : ABindGroupLayoutEntry(name)
    {
        this->getEntry().sampler.type = wgpu::SamplerBindingType::Undefined;
    }
    ~SamplerBindGroupLayoutEntry() = default;

    SamplerBindGroupLayoutEntry(const SamplerBindGroupLayoutEntry &other) = default;
    SamplerBindGroupLayoutEntry &operator=(const SamplerBindGroupLayoutEntry &other) = default;

    std::vector<ValidationError> validate(void) const override
    {
        std::vector<ValidationError> errors = ABindGroupLayoutEntry::validate();
        if (!this->_isSamplerTypeSet)
        {
            errors.push_back({"Sampler type is not set", fmt::format("SamplerBindGroupLayoutEntry({})", this->getName()),
                              ValidationError::Severity::Error});
        }
        return errors;
    }

    inline SamplerBindGroupLayoutEntry &setSamplerType(const wgpu::SamplerBindingType &type)
    {
        this->getEntry().sampler.type = type;
        this->_isSamplerTypeSet = true;
        return *this;
    }

  private:
    bool _isSamplerTypeSet = false;
};
} // namespace Graphic::Utils
