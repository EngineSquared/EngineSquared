#pragma once

#include "exception/UnsupportedTextureFormatError.hpp"
#include "resource/Context.hpp"
#include "resource/Image.hpp"
#include "utils/GetBytesPerPixel.hpp"
#include "utils/webgpu.hpp"
#include <array>
#include <bit>
#include <functional>
#include <glm/gtc/packing.hpp>
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

/**
 * @brief Callback invoked when a readback buffer mapping completes; converts mapped texture data into RGBA8 pixels
 * stored in the provided CallbackData.
 *
 * @param status Result of the mapAsync operation.
 * @param message Optional message produced by the mapping operation.
 * @param userdata1 Pointer to a CallbackData instance that will receive the resulting Image pixels and control flags
 * (must be a valid CallbackData*).
 * @param userdata2 Unused.
 *
 * If @p status is not success the function logs the failure, sets CallbackData::done to true and returns. If mapping
 * succeeded the function reads the mapped buffer, iterates over texels while skipping per-row padding, converts each
 * texel to a 4-channel 8-bit RGBA pixel according to CallbackData::format, appends pixels to CallbackData::data.pixels,
 * unmaps the buffer, and marks CallbackData::done true. Supported source formats: RGBA8UnormSrgb, RGBA8Unorm,
 * BGRA8Unorm, RGBA16Float (per-channel half floats unpacked, clamped to [0,1], and scaled to 0–255), and Depth32Float
 * (depth mapped to a grayscale RGBA with alpha=255).
 *
 * @throws Exception::UnsupportedTextureFormatError If the texture format in CallbackData is not supported for
 * retrieval.
 */
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
        switch (data->format)
        {
        case wgpu::TextureFormat::RGBA8UnormSrgb:
        case wgpu::TextureFormat::RGBA8Unorm:
            pixel.r = mapped[i * 4 + 0];
            pixel.g = mapped[i * 4 + 1];
            pixel.b = mapped[i * 4 + 2];
            pixel.a = mapped[i * 4 + 3];
            break;
        case wgpu::TextureFormat::BGRA8UnormSrgb:
        case wgpu::TextureFormat::BGRA8Unorm:
            pixel.b = mapped[i * 4 + 0];
            pixel.g = mapped[i * 4 + 1];
            pixel.r = mapped[i * 4 + 2];
            pixel.a = mapped[i * 4 + 3];
            break;
        case wgpu::TextureFormat::RGBA16Float: {
            auto r = std::bit_cast<uint16_t>(std::array<uint8_t, 2>{mapped[i * 8 + 0], mapped[i * 8 + 1]});
            auto g = std::bit_cast<uint16_t>(std::array<uint8_t, 2>{mapped[i * 8 + 2], mapped[i * 8 + 3]});
            auto b = std::bit_cast<uint16_t>(std::array<uint8_t, 2>{mapped[i * 8 + 4], mapped[i * 8 + 5]});
            auto a = std::bit_cast<uint16_t>(std::array<uint8_t, 2>{mapped[i * 8 + 6], mapped[i * 8 + 7]});
            pixel.r = static_cast<uint8_t>(std::clamp(glm::unpackHalf1x16(r), 0.0f, 1.0f) * 255.0f);
            pixel.g = static_cast<uint8_t>(std::clamp(glm::unpackHalf1x16(g), 0.0f, 1.0f) * 255.0f);
            pixel.b = static_cast<uint8_t>(std::clamp(glm::unpackHalf1x16(b), 0.0f, 1.0f) * 255.0f);
            pixel.a = static_cast<uint8_t>(std::clamp(glm::unpackHalf1x16(a), 0.0f, 1.0f) * 255.0f);
            break;
        }
        case wgpu::TextureFormat::Depth32Float: {
            auto depth = std::bit_cast<float>(
                std::array<uint8_t, 4>{mapped[i * 4 + 0], mapped[i * 4 + 1], mapped[i * 4 + 2], mapped[i * 4 + 3]});
            auto depthByte = static_cast<uint8_t>(std::clamp(depth, 0.0f, 1.0f) * 255.0f);
            pixel = glm::u8vec4(depthByte, depthByte, depthByte, 255);
            break;
        }
        default: throw Exception::UnsupportedTextureFormatError("Texture format not supported for retrieval.");
        }
        data->data.pixels.push_back(pixel);
    }
    buf.unmap();
    data->done = true;
};
class Texture {
  public:
    Texture(std::string_view name, wgpu::Texture texture, bool ownsResources = true)
        : _webgpuTexture(texture), _defaultView(nullptr), _name(std::string(name)), _ownsResources(ownsResources)
    {
        _defaultView = _webgpuTexture.createView();
    }

    Texture(const Context &context, const wgpu::TextureDescriptor &descriptor)
        : Texture(std::string(descriptor.label.data, descriptor.label.length),
                  context.deviceContext.GetDevice()->createTexture(descriptor))
    {
    }

    Texture(const Context &context, std::string_view name, const Image &image)
        : Texture(context, _BuildDescriptor(name, image))
    {
        Write(context, image);
    }

    Texture(const Context &context, std::string_view name, const glm::uvec2 &size,
            const std::function<glm::u8vec4(glm::uvec2 pos)> &callback)
        : Texture(context, name, Image(size, callback))
    {
    }

    ~Texture()
    {
        if (_defaultView != nullptr)
            _defaultView.release();

        if (_ownsResources && _webgpuTexture != nullptr)
            _webgpuTexture.release();
    }

    Texture(const Texture &) = delete;
    Texture &operator=(const Texture &) = delete;

    Texture(Texture &&other) noexcept
        : _webgpuTexture(std::move(other._webgpuTexture)), _defaultView(std::move(other._defaultView)),
          _name(std::move(other._name)), _ownsResources(other._ownsResources)
    {
        other._webgpuTexture = nullptr;
        other._defaultView = nullptr;
        other._name.clear();
        other._ownsResources = false;
    }

    Texture &operator=(Texture &&other) noexcept
    {
        if (this != &other)
        {
            if (_defaultView != nullptr)
                _defaultView.release();

            if (_ownsResources && _webgpuTexture != nullptr)
            {
                _webgpuTexture.release();
            }

            _webgpuTexture = std::move(other._webgpuTexture);
            _defaultView = std::move(other._defaultView);
            _name = std::move(other._name);
            _ownsResources = other._ownsResources;

            other._webgpuTexture = nullptr;
            other._defaultView = nullptr;
            other._name.clear();
            other._ownsResources = false;
        }
        return *this;
    }

    inline glm::uvec2 GetSize() const { return glm::uvec2{_webgpuTexture.getWidth(), _webgpuTexture.getHeight()}; }

    // We assume the image is correctly formatted (width * height = pixels.size())
    void Write(const Context &context, const Image &image)
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
        const wgpu::Queue &queue = context.queue.value();
        queue.writeTexture(destination, image.pixels.data(), source.bytesPerRow * source.rowsPerImage, source,
                           textureSize);
    }

    /**
     * @brief Reads back the GPU texture and returns it as an Image.
     *
     * Copies the texture to a CPU-readable buffer, converts the source texel format into 4-channel
     * RGBA byte pixels, and returns an Image populated with those pixels. Depth formats are
     * mapped to grayscale RGBA (depth -> luminance, alpha = 255).
     *
     * @return Image The retrieved image with width and height matching the texture and 4 channels (RGBA).
     */
    Image RetrieveImage(const Context &context) const
    {
        const wgpu::Queue &queue = context.queue.value();

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
        if (_webgpuTexture.getFormat() == wgpu::TextureFormat::Depth24Plus ||
            _webgpuTexture.getFormat() == wgpu::TextureFormat::Depth24PlusStencil8 ||
            _webgpuTexture.getFormat() == wgpu::TextureFormat::Depth32Float ||
            _webgpuTexture.getFormat() == wgpu::TextureFormat::Depth32FloatStencil8)
        {
            srcView.aspect = wgpu::TextureAspect::DepthOnly;
        }
        else
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

    void ReleaseOwnership() { _ownsResources = false; }

    void TakeOwnership() { _ownsResources = true; }

    bool OwnsResources() const { return _ownsResources; }

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
        textureDesc.format = wgpu::TextureFormat::RGBA8UnormSrgb;
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
    bool _ownsResources = true;
};
} // namespace Graphic::Resource
