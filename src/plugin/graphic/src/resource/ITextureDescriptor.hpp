#pragma once

#include <filesystem>
#include "resource/Context.hpp"
#include "stb_image.h"
#include <glm/vec3.hpp>
#include "utils/webgpu.hpp"
#include <optional>

namespace Graphic::Resource
{
    struct ImageData {
        uint32_t width;
        uint32_t height;
        int channels;
        unsigned char *pixels;
    };

    class ITextureDescriptor
    {
    public:
        virtual ~ITextureDescriptor() = default;

        virtual const std::string &GetName(void) const = 0;
        virtual const glm::ivec3 &GetSize(void) const = 0;
        virtual const wgpu::TextureDimension &GetDimension(void) const = 0;
        virtual const uint32_t &GetMipLevelCount(void) const = 0;
        virtual const uint32_t &GetSampleCount(void) const = 0;
        virtual const wgpu::TextureFormat &GetFormat(void) const = 0;
        virtual const wgpu::TextureUsage &GetUsage(void) const = 0;
        virtual const std::optional<ImageData> &GetImageData(void) const = 0;
    };
} // namespace Graphic::Resource

