#pragma once

#include "ITextureDescriptor.hpp"
#include "resource/Context.hpp"
#include "stb_image.h"
#include <filesystem>

namespace Graphic::Resource {
class Texture {
  public:
    ~Texture() = default;

    template <typename TTextureDescriptor,
              typename = std::enable_if_t<std::is_base_of_v<ITextureDescriptor, TTextureDescriptor>>>
    static Texture Create(Context &context, const TTextureDescriptor &descriptor)
    {
        Texture texture;

        wgpu::TextureDescriptor textureDesc{};
        textureDesc.size.width = static_cast<uint32_t>(descriptor.GetSize().x);
        textureDesc.size.height = static_cast<uint32_t>(descriptor.GetSize().y);
        textureDesc.size.depthOrArrayLayers = static_cast<uint32_t>(descriptor.GetSize().z);
        textureDesc.dimension = descriptor.GetDimension();
        textureDesc.mipLevelCount = descriptor.GetMipLevelCount();
        textureDesc.sampleCount = descriptor.GetSampleCount();
        textureDesc.format = descriptor.GetFormat();
        textureDesc.usage = descriptor.GetUsage();
        textureDesc.label = wgpu::StringView(descriptor.GetName().c_str());
        texture._webgpuTexture = context.deviceContext.GetDevice()->createTexture(textureDesc);

        if (descriptor.GetImageData().has_value())
        {
            texture.WriteTexture(context, descriptor.GetImageData().value());
        }

        return texture;
    }

    void WriteTexture(Context &context, const ImageData &imageData)
    {
        if (!imageData.pixels)
        {
            throw std::runtime_error("No pixel data to write to texture."); // TODO: Custom exception
        }
        if (imageData.width != this->_webgpuTexture.getWidth() || imageData.height != this->_webgpuTexture.getHeight())
        {
            Log::Warn("Image data size does not match texture size.");
        }

        wgpu::Extent3D textureSize = {this->_webgpuTexture.getWidth(), this->_webgpuTexture.getHeight(), 1};
        wgpu::TexelCopyTextureInfo destination;
        destination.texture = this->_webgpuTexture;
        destination.mipLevel = 0;
        destination.origin = {0, 0, 0};
        destination.aspect = wgpu::TextureAspect::All;

        wgpu::TexelCopyBufferLayout source;
        source.offset = 0;
        source.bytesPerRow = imageData.channels * textureSize.width;
        source.rowsPerImage = textureSize.height;
        wgpu::Queue &queue = context.queue.value();
        queue.writeTexture(destination, imageData.pixels, source.bytesPerRow * source.rowsPerImage, source,
                           textureSize);
    }

    auto GetDataTexture(Context &context) const -> std::vector<unsigned char>
    {
        wgpu::Queue &queue = context.queue.value();

        wgpu::Extent3D copySize(_webgpuTexture.getWidth(), _webgpuTexture.getHeight(), 1);

        wgpu::BufferDescriptor bufDesc(wgpu::Default);
        bufDesc.size = copySize.width * copySize.height * getBytesPerPixel();
        bufDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::MapRead;
        bufDesc.label = wgpu::StringView("Depth Readback Buffer");
        wgpu::Buffer readbackBuffer = context.deviceContext.GetDevice()->createBuffer(bufDesc);

        wgpu::CommandEncoder encoder = context.deviceContext.GetDevice()->createCommandEncoder();
        wgpu::TexelCopyTextureInfo srcView(wgpu::Default);
        srcView.texture = _webgpuTexture;
        srcView.mipLevel = 0;
        srcView.origin = {0, 0, 0};
        srcView.aspect = wgpu::TextureAspect::DepthOnly;

        wgpu::TexelCopyBufferInfo dstView(wgpu::Default);
        dstView.buffer = readbackBuffer;
        dstView.layout.offset = 0;
        dstView.layout.bytesPerRow = copySize.width * getBytesPerPixel();
        dstView.layout.rowsPerImage = copySize.height;

        encoder.copyTextureToBuffer(srcView, dstView, copySize);
        auto cmd = encoder.finish();
        queue.submit(1, &cmd);
        cmd.release();

        struct CallbackData {
            wgpu::Buffer buffer;
            std::vector<unsigned char> data;
        } cbData;

        wgpu::BufferMapCallbackInfo cbInfo(wgpu::Default);
        cbInfo.mode = wgpu::CallbackMode::AllowSpontaneous;
        cbInfo.callback = +[](WGPUMapAsyncStatus status, WGPUStringView /*message*/, void *userdata1, void *userdata2) {
            CallbackData *data = reinterpret_cast<CallbackData *>(userdata1);
            if (status != wgpu::MapAsyncStatus::Success)
            {
                return;
            }
            auto &buf = data->buffer;
            uint8_t *mapped = static_cast<uint8_t *>(buf.getMappedRange(0, buf.getSize()));
            data->data.assign(mapped, mapped + buf.getSize());
            buf.unmap();
        };
        cbInfo.userdata1 = &cbData;
        cbInfo.userdata2 = nullptr;
        readbackBuffer.mapAsync(wgpu::MapMode::Read, 0, readbackBuffer.getSize(), cbInfo);
        while (cbData.data.empty())
        {
            context.instance->processEvents();
        }
        readbackBuffer.release();
        return cbData.data;
    }

  private:
    uint32_t getBytesPerPixel() const
    {
        switch (_webgpuTexture.getFormat())
        {
        case wgpu::TextureFormat::RGBA8Unorm:
        case wgpu::TextureFormat::RGBA8UnormSrgb:
        case wgpu::TextureFormat::BGRA8Unorm:
        case wgpu::TextureFormat::BGRA8UnormSrgb:
        case wgpu::TextureFormat::RGBA8Uint:
        case wgpu::TextureFormat::RGBA8Sint: return 4;
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
        default: throw std::runtime_error("Unsupported texture format for byte size calculation.");
        }
    }

    Texture(void) = default;

    wgpu::Texture _webgpuTexture;
};
} // namespace Graphic::Resource
