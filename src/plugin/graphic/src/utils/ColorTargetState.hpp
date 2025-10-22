#pragma once

#include "spdlog/fmt/fmt.h"
#include "utils/IValidable.hpp"
#include "utils/webgpu.hpp"

namespace Graphic::Utils {
class ColorTargetState : public virtual IValidable {
  public:
    ColorTargetState(const std::string &name) : name(name) {}
    ~ColorTargetState() = default;

    inline ColorTargetState &setFormat(wgpu::TextureFormat format)
    {
        this->format = format;
        return *this;
    }

    inline wgpu::TextureFormat getFormat(void) const { return this->format; }

    std::vector<ValidationError> validate(void) const override
    {
        std::vector<ValidationError> errors;
        if (this->format == wgpu::TextureFormat::Undefined)
        {
            errors.push_back({"Format is not set", fmt::format("ColorTargetState({})", this->name),
                              ValidationError::Severity::Error});
        }
        return errors;
    }

  private:
    wgpu::TextureFormat format = wgpu::TextureFormat::Undefined;
    std::string name;
};
} // namespace Graphic::Utils
