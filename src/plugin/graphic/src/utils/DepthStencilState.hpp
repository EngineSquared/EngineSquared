#pragma once

#include "utils/webgpu.hpp"
#include "utils/IValidable.hpp"
#include "spdlog/fmt/fmt.h"

namespace Plugin::Graphic::Utils {
    class DepthStencilState : public virtual IValidable {
        public:
            DepthStencilState(const std::string &name) : name(name) {}
            ~DepthStencilState() = default;

            std::vector<ValidationError> validate(void) const override {
                std::vector<ValidationError> errors;
                if (this->value.format == wgpu::TextureFormat::Undefined) {
                    errors.push_back({ "Format is not set", fmt::format("DepthStencilState({})", this->name), ValidationError::Severity::Error });
                }
                if (this->value.depthWriteEnabled && this->value.depthCompare == wgpu::CompareFunction::Undefined) {
                    errors.push_back({ "Depth compare function is not set while depth write is enabled", fmt::format("DepthStencilState({})", this->name), ValidationError::Severity::Error });
                }
                return errors;
            }

            inline DepthStencilState &setCompareFunction(wgpu::CompareFunction func) {
                this->value.depthCompare = func;
                return *this;
            }

            inline DepthStencilState &setDepthWriteEnabled(wgpu::OptionalBool enabled) {
                this->value.depthWriteEnabled = enabled;
                return *this;
            }

            inline DepthStencilState &setFormat(wgpu::TextureFormat format) {
                this->value.format = format;
                return *this;
            }

        private:
            wgpu::DepthStencilState value = wgpu::DepthStencilState(wgpu::Default);
            std::string name;
    };
}
