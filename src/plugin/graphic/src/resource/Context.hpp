#pragma once

#include "utils/webgpu.hpp"

namespace Plugin::Graphic::Resource {
class Context {
    public:
        Context() = default;
        ~Context() = default;

        std::optional<wgpu::Instance> instance;
};
}
