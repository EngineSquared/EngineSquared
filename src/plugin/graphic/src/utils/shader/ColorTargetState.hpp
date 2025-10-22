#pragma once

#include "spdlog/fmt/fmt.h"
#include "utils/IValidable.hpp"
#include "utils/webgpu.hpp"

namespace Graphic::Utils {
class ColorTargetState : public IValidable {
  public:
    explicit ColorTargetState(const std::string &name) : _name(name) {}
    ~ColorTargetState() override = default;

    inline ColorTargetState &setFormat(wgpu::TextureFormat format)
    {
        this->_format = format;
        return *this;
    }

    inline wgpu::TextureFormat getFormat(void) const { return this->_format; }

    std::vector<ValidationError> validate(void) const override
    {
        std::vector<ValidationError> errors;
        if (this->_format == wgpu::TextureFormat::Undefined)
        {
            errors.emplace_back("Format is not set", fmt::format("ColorTargetState({})", this->_name),
                              ValidationError::Severity::Error);
        }
        return errors;
    }

  private:
    wgpu::TextureFormat _format = wgpu::TextureFormat::Undefined;
    std::string _name;
};
} // namespace Graphic::Utils
