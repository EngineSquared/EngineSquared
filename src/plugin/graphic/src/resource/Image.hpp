#pragma once

#include "lodepng.h"
#include "stb_image.h"
#include <filesystem>
#include <glm/vec4.hpp>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace Graphic::Resource {

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
        if (!data)
            throw std::runtime_error("Failed to load image data from file: " +
                                     filepath.string()); // TODO: Custom exception

        image.width = static_cast<uint32_t>(width_);
        image.height = static_cast<uint32_t>(height_);
        image.channels = 4;

        image.pixels.resize(width_ * height_);

        for (uint32_t y = 0; y < image.height; ++y)
        {
            for (uint32_t x = 0; x < image.width; ++x)
            {
                size_t index = y * image.width + x;
                image.pixels[index] =
                    glm::u8vec4(data[index * 4 + 0], data[index * 4 + 1], data[index * 4 + 2], data[index * 4 + 3]);
            }
        }

        stbi_image_free(data);
        return image;
    }

    int ToPng(std::string_view filename)
    {
        return lodepng::encode(filename.data(), reinterpret_cast<const unsigned char *>(pixels.data()), width, height);
    }
};
}; // namespace Graphic::Resource
