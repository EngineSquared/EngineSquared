#pragma once

#include "exception/UnsetAttributeTextureDescriptor.hpp"
#include "resource/ITextureDescriptor.hpp"
#include "stb_image.h"
#include <optional>

namespace Graphic::Resource {
class FileTextureDescriptor : public ITextureDescriptor {
  public:
    FileTextureDescriptor(void) = default;
    FileTextureDescriptor(const std::string &name) : _name(name){};
    virtual ~FileTextureDescriptor() { UnloadFile(); }

    FileTextureDescriptor(const FileTextureDescriptor &other) = delete;
    FileTextureDescriptor &operator=(const FileTextureDescriptor &other) = delete;

    FileTextureDescriptor &SetName(const std::string &name)
    {
        _name = name;
        return *this;
    }
    FileTextureDescriptor &LoadFile(const std::filesystem::path &filepath)
    {
        int width, height, channels;

        // If filePath don't exist, throw an error
        if (!std::filesystem::exists(filepath))
        {
            throw std::runtime_error("Texture file does not exist: " + filepath.string()); // TODO: Custom exception
        }

        unsigned char *pixelData =
            stbi_load(filepath.string().c_str(), &width, &height, &channels, 4 /* force 4 channels */);
        if (!pixelData)
            throw std::runtime_error("Failed to load texture data.");

        _size = glm::ivec3(width, height, 1);
        _dimension = wgpu::TextureDimension::_2D;
        _mipLevelCount = 1;
        _sampleCount = 1;
        _format = wgpu::TextureFormat::RGBA8Unorm;
        _usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::CopyDst;
        ImageData imageData;
        imageData.width = width;
        imageData.height = height;
        imageData.channels = 4;
        imageData.pixels = pixelData;
        _imageData = imageData;
        return *this;
    }

    void UnloadFile()
    {
        if (_imageData.has_value() && _imageData->pixels)
        {
            stbi_image_free(_imageData->pixels);
            _imageData.reset();
        }
    }

    const std::string &GetName(void) const override { return _name.has_value() ? _name.value() : _DEFAULT_NAME; }
    const glm::ivec3 &GetSize(void) const override
    {
        if (!_size.has_value())
            throw Exception::UnsetAttributeTextureDescriptor("Size attribute is not set");
        return _size.value();
    }
    const wgpu::TextureDimension &GetDimension(void) const override
    {
        if (!_dimension.has_value())
            throw Exception::UnsetAttributeTextureDescriptor("Dimension attribute is not set");
        return _dimension.value();
    }
    const uint32_t &GetMipLevelCount(void) const override
    {
        if (!_mipLevelCount.has_value())
            throw Exception::UnsetAttributeTextureDescriptor("MipLevelCount attribute is not set");
        return _mipLevelCount.value();
    }
    const uint32_t &GetSampleCount(void) const override
    {
        if (!_sampleCount.has_value())
            throw Exception::UnsetAttributeTextureDescriptor("SampleCount attribute is not set");
        return _sampleCount.value();
    }
    const wgpu::TextureFormat &GetFormat(void) const override
    {
        if (!_format.has_value())
            throw Exception::UnsetAttributeTextureDescriptor("Format attribute is not set");
        return _format.value();
    }
    const wgpu::TextureUsage &GetUsage(void) const override
    {
        if (!_usage.has_value())
            throw Exception::UnsetAttributeTextureDescriptor("Usage attribute is not set");
        return _usage.value();
    }

    const std::optional<ImageData> &GetImageData(void) const override { return _imageData; }

  private:
    inline const static std::string _DEFAULT_NAME = "UnnamedTexture";

    std::optional<std::string> _name;
    std::optional<glm::ivec3> _size;
    std::optional<wgpu::TextureDimension> _dimension;
    std::optional<uint32_t> _mipLevelCount;
    std::optional<uint32_t> _sampleCount;
    std::optional<wgpu::TextureFormat> _format;
    std::optional<wgpu::TextureUsage> _usage;
    std::optional<ImageData> _imageData;
};
} // namespace Graphic::Resource
