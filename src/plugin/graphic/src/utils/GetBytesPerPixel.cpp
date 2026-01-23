#include "utils/GetBytesPerPixel.hpp"
#include "exception/UnsupportedTextureFormatError.hpp"

namespace Graphic::Utils {

/**
 * @brief Determines the number of bytes per pixel for a given wgpu texture format.
 *
 * @param format The wgpu::TextureFormat to evaluate.
 * @return uint32_t Number of bytes stored per pixel for the specified format.
 * @throws Exception::UnsupportedTextureFormatError If the format is not supported by this mapping.
 */
uint32_t GetBytesPerPixel(wgpu::TextureFormat format)
{
    switch (format)
    {
    case wgpu::TextureFormat::RGBA16Float: return 8;
    case wgpu::TextureFormat::RGBA8Unorm:
    case wgpu::TextureFormat::RGBA8UnormSrgb:
    case wgpu::TextureFormat::BGRA8Unorm:
    case wgpu::TextureFormat::BGRA8UnormSrgb:
    case wgpu::TextureFormat::RGBA8Uint:
    case wgpu::TextureFormat::RGBA8Sint:
    case wgpu::TextureFormat::Depth32Float:
    case wgpu::TextureFormat::RGB10A2Uint:
    case wgpu::TextureFormat::RGB10A2Unorm: return 4;
    case wgpu::TextureFormat::RG8Unorm:
    case wgpu::TextureFormat::RG8Snorm:
    case wgpu::TextureFormat::RG8Uint:
    case wgpu::TextureFormat::RG8Sint: return 2;
    case wgpu::TextureFormat::R8Unorm:
    case wgpu::TextureFormat::R8Snorm:
    case wgpu::TextureFormat::R8Uint:
    case wgpu::TextureFormat::R8Sint: return 1;
    // Add more formats as needed
    default: throw Exception::UnsupportedTextureFormatError("Unsupported texture format for byte size calculation.");
    }
}
} // namespace Graphic::Utils