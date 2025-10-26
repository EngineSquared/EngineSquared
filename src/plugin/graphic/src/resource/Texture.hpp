#pragma once

#include "ITextureDescriptor.hpp"
#include "resource/Context.hpp"
#include "stb_image.h"
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <lodepng.h>

namespace Graphic::Resource
{
    struct Image {
        uint32_t width;
        uint32_t height;
        int channels;
        std::vector<glm::u8vec4> pixels;

        static Image LoadFromFile(const std::filesystem::path &filepath)
        {
            Image image;
            int width_, height_, channels_;
            unsigned char *data = stbi_load(filepath.string().c_str(), &width_, &height_, &channels_, 4);
            if (!data) throw std::runtime_error("Failed to load image data from file: " + filepath.string()); // TODO: Custom exception

            image.width = static_cast<uint32_t>(width_);
            image.height = static_cast<uint32_t>(height_);
            image.channels = 4;

            image.pixels.resize(width_ * height_);

            for (uint32_t y = 0; y < image.height; ++y) {
                for (uint32_t x = 0; x < image.width; ++x) {
                    size_t index = y * image.width + x;
                    image.pixels[index] = glm::u8vec4(
                        data[index * 4 + 0],
                        data[index * 4 + 1],
                        data[index * 4 + 2],
                        data[index * 4 + 3]
                    );
                }
            }

            stbi_image_free(data);
            return image;
        }

        int ToPng(std::string_view filename) {
            return lodepng::encode(filename.data(), reinterpret_cast<const unsigned char *>(pixels.data()), width, height);
        }
    };

    struct ImageFile : public Image {
        std::filesystem::path filepath;
    };

    class Texture
    {
        public:
        static Texture Create(Context &context, const wgpu::TextureDescriptor &descriptor)
        {
            Texture texture;

            texture._webgpuTexture = context.deviceContext.GetDevice()->createTexture(descriptor);
            texture._name = std::string(descriptor.label.data, descriptor.label.length);

            return texture;
        }

        static Texture Create(Context &context, std::string_view name, const Image &image)
        {
            Texture texture;
            texture._name = std::string(name.data(), name.length());

            wgpu::TextureDescriptor textureDesc;
            textureDesc.label = wgpu::StringView(name.data());
            textureDesc.size = { image.width, image.height, 1 };
            textureDesc.dimension = wgpu::TextureDimension::_2D;
            textureDesc.mipLevelCount = 1;
            textureDesc.sampleCount = 1;
            textureDesc.format = wgpu::TextureFormat::RGBA8Unorm;
            textureDesc.usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::RenderAttachment | wgpu::TextureUsage::CopySrc | wgpu::TextureUsage::CopyDst;
            textureDesc.viewFormats = nullptr;
            textureDesc.viewFormatCount = 0;

            texture = Create(context, textureDesc);

            texture.WriteTexture(context, image);

            return texture;
        }

        static Texture Create(Context &context, std::string_view name, const glm::uvec2 &size, const std::function<glm::u8vec4 (glm::uvec2 pos)> &callback)
        {
            Texture texture;

            wgpu::TextureDescriptor textureDesc;
            textureDesc.label = wgpu::StringView(name.data());
            textureDesc.size = { size.x, size.y, 1 };
            textureDesc.dimension = wgpu::TextureDimension::_2D;
            textureDesc.mipLevelCount = 1;
            textureDesc.sampleCount = 1;
            textureDesc.format = wgpu::TextureFormat::RGBA8Unorm;
            textureDesc.usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::CopyDst;
            textureDesc.viewFormats = nullptr;
            textureDesc.viewFormatCount = 0;

            texture = Create(context, textureDesc);
            Image image;
            image.width = size.x;
            image.height = size.y;
            image.channels = 4;
            image.pixels.resize(size.x * size.y * image.channels);

            for (uint32_t y = 0; y < size.y; ++y) {
                for (uint32_t x = 0; x < size.x; ++x) {
                    glm::uvec2 pos{x, y};
                    image.pixels[y * size.x + x] = callback(pos);
                }
            }

            texture.WriteTexture(context, image);

            return texture;
        }

        // We assume the image is correctly formatted (width * height = pixels.size())
        void WriteTexture(Context &context, const Image &image) {
            if (image.width != this->_webgpuTexture.getWidth() ||
                image.height != this->_webgpuTexture.getHeight()) {
                Log::Warn("Image data size does not match texture size.");
            }

            wgpu::Extent3D textureSize = { this->_webgpuTexture.getWidth(), this->_webgpuTexture.getHeight(), 1 };
            wgpu::TexelCopyTextureInfo destination;
            destination.texture = this->_webgpuTexture;
            destination.mipLevel = 0;
            destination.origin = { 0, 0, 0 };
            destination.aspect = wgpu::TextureAspect::All;

            wgpu::TexelCopyBufferLayout source;
            source.offset = 0;
            source.bytesPerRow = image.channels * textureSize.width;
            source.rowsPerImage = textureSize.height;
            wgpu::Queue &queue = context.queue.value();
            queue.writeTexture(destination, image.pixels.data(), source.bytesPerRow * source.rowsPerImage, source, textureSize);
        }

        auto GetDataTexture(Context &context) const
            -> Image
        {
            wgpu::Queue &queue = context.queue.value();

            wgpu::Extent3D copySize(_webgpuTexture.getWidth(), _webgpuTexture.getHeight(), 1);

            wgpu::CommandEncoder encoder = context.deviceContext.GetDevice()->createCommandEncoder();

            const uint32_t bytesPerRow = (copySize.width * getBytesPerPixel() + 255) / 256 * 256;
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

            struct CallbackData {
                wgpu::Buffer buffer;
                Image data;
                uint32_t bytesPerRow;
                bool done = false;
            };
            CallbackData cbData = {readbackBuffer, {}, bytesPerRow, false};
            cbData.data.width = copySize.width;
            cbData.data.height = copySize.height;
            cbData.data.channels = 4;

            wgpu::BufferMapCallbackInfo cbInfo(wgpu::Default);
            cbInfo.mode = wgpu::CallbackMode::AllowSpontaneous;
            cbInfo.callback = [](WGPUMapAsyncStatus status, WGPUStringView message,
                         void *userdata1, void *userdata2) {
                CallbackData *data = reinterpret_cast<CallbackData *>(userdata1);
                if (status != wgpu::MapAsyncStatus::Success) {
                    Log::Error(fmt::format("Failed to map buffer: {}", std::string_view(message.data, message.length)));
                    data->done = true;
                    return;
                }
                auto &buf = data->buffer;
                uint8_t *mapped = static_cast<uint8_t *>(buf.getMappedRange(0, buf.getSize()));
                uint32_t size = (data->bytesPerRow / 4) * data->data.height;
                for (size_t i = 0; i < size; i++) {
                    if (i % (data->bytesPerRow / 4) >= data->data.width) {
                        continue; // Skip padding bytes
                    }
                    glm::u8vec4 pixel;
                    pixel.r = mapped[i * 4 + 0];
                    pixel.g = mapped[i * 4 + 1];
                    pixel.b = mapped[i * 4 + 2];
                    pixel.a = 255;
                    data->data.pixels.push_back(pixel);
                }
                buf.unmap();
                data->done = true;
            };
            cbInfo.userdata1 = &cbData;
            cbInfo.userdata2 = nullptr;
            readbackBuffer.mapAsync(wgpu::MapMode::Read, 0, readbackBuffer.getSize(), cbInfo);
            while (!cbData.done) {
                context.deviceContext.GetDevice()->poll(false, nullptr);
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
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

        wgpu::Texture _webgpuTexture;
        std::string _name;
    };
} // namespace Graphic::Resource
