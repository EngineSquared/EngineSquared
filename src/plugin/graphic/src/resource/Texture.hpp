#pragma once

#include "exception/UnsupportedTextureFormatError.hpp"
#include "resource/Context.hpp"
#include "resource/Image.hpp"
#include "utils/GetBytesPerPixel.hpp"
#include "utils/webgpu.hpp"
#include <functional>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

namespace Graphic::Resource {

struct CallbackData {
    wgpu::Buffer buffer;
    Image data;
    uint32_t bytesPerRow;
    wgpu::TextureFormat format;
    bool done = false;
};

static void TextureRetrieveCallback(WGPUMapAsyncStatus status, WGPUStringView message, void *userdata1, void *userdata2)
{
    auto data = static_cast<CallbackData *>(userdata1);
    if (status != wgpu::MapAsyncStatus::Success)
    {
        Log::Error(fmt::format("Failed to map buffer: {}", std::string_view(message.data, message.length)));
        data->done = true;
        return;
    }
    auto &buf = data->buffer;
    auto mapped = static_cast<uint8_t *>(buf.getMappedRange(0, buf.getSize()));
    uint32_t size = (data->bytesPerRow / Graphic::Utils::GetBytesPerPixel(data->format)) * data->data.height;
    for (size_t i = 0; i < size; i++)
    {
        if (i % (data->bytesPerRow / Graphic::Utils::GetBytesPerPixel(data->format)) >= data->data.width)
        {
            continue; // Skip padding bytes
        }
        glm::u8vec4 pixel;
        // Here we assume the texture format is RGBA8Unorm (4 bytes per pixel)
        pixel.r = mapped[i * 4 + 0];
        pixel.g = mapped[i * 4 + 1];
        pixel.b = mapped[i * 4 + 2];
        pixel.a = mapped[i * 4 + 3];
        data->data.pixels.push_back(pixel);
    }
    buf.unmap();
    data->done = true;
};
class Texture {
  public:
    Texture(Context &context, const wgpu::TextureDescriptor &descriptor) : _name(std::string(descriptor.label.data, descriptor.label.length))
    {
        _webgpuTexture = context.deviceContext.GetDevice()->createTexture(descriptor);
        _defaultView = _webgpuTexture.createView();
    }

    Texture(Context &context, std::string_view name, const Image &image)
        : Texture(context, _BuildDescriptor(name, image))
    {
        Write(context, image);
    }

    Texture(Context &context, std::string_view name, const glm::uvec2 &size,
            const std::function<glm::u8vec4(glm::uvec2 pos)> &callback)
        : Texture(context, name, Image(size, callback))
    {
    }

    // We assume the image is correctly formatted (width * height = pixels.size())
    void Write(Context &context, const Image &image)
    {
        if (image.width != this->_webgpuTexture.getWidth() || image.height != this->_webgpuTexture.getHeight())
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
        source.bytesPerRow = image.channels * textureSize.width;
        source.rowsPerImage = textureSize.height;
        wgpu::Queue &queue = context.queue.value();
        queue.writeTexture(destination, image.pixels.data(), source.bytesPerRow * source.rowsPerImage, source,
                           textureSize);
    }

    Image RetrieveImage(Context &context) const
    {
        wgpu::Queue &queue = context.queue.value();

        wgpu::Extent3D copySize(_webgpuTexture.getWidth(), _webgpuTexture.getHeight(), 1);

        wgpu::CommandEncoder encoder = context.deviceContext.GetDevice()->createCommandEncoder();

        const uint32_t bytesPerRow = (copySize.width * _GetBytesPerPixel() + 255) / 256 * 256;
        wgpu::BufferDescriptor bufDesc(wgpu::Default);
        bufDesc.size = copySize.height * bytesPerRow;
        bufDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::MapRead;
        bufDesc.label = wgpu::StringView("Depth Readback Buffer");
        wgpu::Buffer readbackBuffer = context.deviceContext.GetDevice()->createBuffer(bufDesc);

        wgpu::TexelCopyTextureInfo srcView(wgpu::Default);
        srcView.texture = _webgpuTexture;
        srcView.mipLevel = 0;
        srcView.origin = {0, 0, 0};
        srcView.aspect = wgpu::TextureAspect::All;

        wgpu::TexelCopyBufferInfo dstView(wgpu::Default);
        dstView.buffer = readbackBuffer;
        dstView.layout.offset = 0;
        dstView.layout.bytesPerRow = bytesPerRow;
        dstView.layout.rowsPerImage = copySize.height;

        encoder.copyTextureToBuffer(srcView, dstView, copySize);
        auto cmd = encoder.finish();
        auto cmdName = fmt::format("{} Readback Command", _name);
        queue.submit(1, &cmd);
        cmd.release();

        CallbackData cbData = {readbackBuffer, {}, bytesPerRow, _webgpuTexture.getFormat(), false};
        cbData.data.width = copySize.width;
        cbData.data.height = copySize.height;
        cbData.data.channels = 4;

        wgpu::BufferMapCallbackInfo cbInfo(wgpu::Default);
        cbInfo.mode = wgpu::CallbackMode::AllowSpontaneous;
        cbInfo.callback = TextureRetrieveCallback;
        cbInfo.userdata1 = &cbData;
        cbInfo.userdata2 = nullptr;
        readbackBuffer.mapAsync(wgpu::MapMode::Read, 0, readbackBuffer.getSize(), cbInfo);
        while (!cbData.done)
        {
            context.deviceContext.GetDevice()->poll(false, nullptr);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        return cbData.data;
    }

    wgpu::TextureView GetDefaultView() const { return _defaultView; }

  private:
    Texture(void) = default;

    static wgpu::TextureDescriptor _BuildDescriptor(std::string_view name, const Image &image)
    {
        wgpu::TextureDescriptor textureDesc(wgpu::Default);
        textureDesc.label = wgpu::StringView(name);
        textureDesc.size = {image.width, image.height, 1};
        textureDesc.dimension = wgpu::TextureDimension::_2D;
        textureDesc.mipLevelCount = 1;
        textureDesc.sampleCount = 1;
        textureDesc.format = wgpu::TextureFormat::RGBA8Unorm;
        textureDesc.usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::RenderAttachment |
                            wgpu::TextureUsage::CopySrc | wgpu::TextureUsage::CopyDst;
        textureDesc.viewFormats = nullptr;
        textureDesc.viewFormatCount = 0;
        return textureDesc;
    }

    uint32_t _GetBytesPerPixel() const { return Utils::GetBytesPerPixel(_webgpuTexture.getFormat()); }

    wgpu::Texture _webgpuTexture;
    wgpu::TextureView _defaultView;
    std::string _name;
};
} // namespace Graphic::Resource
