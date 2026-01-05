#pragma once

#include "exception/FileReadingError.hpp"
#include "exception/FileWritingError.hpp"
#include "exception/UnknownFileError.hpp"
#include "lodepng.h"
#include "stb_image.h"
#include <filesystem>
#include <functional>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <string>
#include <string_view>
#include <vector>

namespace Graphic::Resource {

struct Image {
    uint32_t width = 0;
    uint32_t height = 0;
    int channels = 0;
    std::vector<glm::u8vec4> pixels;

    Image() = default;

    template <typename Callback>
    explicit Image(const glm::uvec2 &size, Callback callback) : width(size.x), height(size.y), channels(4)
    {
        this->pixels.resize(size.x * size.y);
        for (uint32_t y = 0; y < size.y; ++y)
        {
            for (uint32_t x = 0; x < size.x; ++x)
            {
                glm::uvec2 pos{x, y};
                this->pixels[y * size.x + x] = callback(pos);
            }
        }
    }

    explicit Image(const std::filesystem::path &filepath)
    {
        // Should we assume the file exists before calling this function?
        if (std::filesystem::exists(filepath) == false)
            throw Exception::UnknownFileError("File not found at: " + filepath.string());

        int width_ = -1;
        int height_ = -1;
        int channels_ = -1;
        unsigned char *data = stbi_load(filepath.string().c_str(), &width_, &height_, &channels_, 4);

        if (!data || width_ <= 0 || height_ <= 0 || channels_ <= 0)
            throw Exception::FileReadingError("Failed to load image data from file: " + filepath.string());

        this->width = static_cast<uint32_t>(width_);
        this->height = static_cast<uint32_t>(height_);
        this->channels = 4;

        this->pixels.resize(width_ * height_);
        for (uint32_t y = 0; y < this->height; ++y)
        {
            for (uint32_t x = 0; x < this->width; ++x)
            {
                size_t index = y * this->width + x;
                this->pixels[index] =
                    glm::u8vec4(data[index * 4 + 0], data[index * 4 + 1], data[index * 4 + 2], data[index * 4 + 3]);
            }
        }

        stbi_image_free(data);
    }

    void ToPng(std::string_view filename)
    {
        unsigned int error =
            lodepng::encode(filename.data(), reinterpret_cast<const unsigned char *>(pixels.data()), width, height);

        if (error != 0)
            throw Exception::FileWritingError(
                fmt::format("Failed to write PNG file '{}': {}", filename, lodepng_error_text(error)));
    }
};
}; // namespace Graphic::Resource
