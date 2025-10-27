#pragma once

#include "utils/webgpu.hpp"

namespace Graphic::Utils {
uint32_t GetBytesPerPixel(wgpu::TextureFormat format);
};
