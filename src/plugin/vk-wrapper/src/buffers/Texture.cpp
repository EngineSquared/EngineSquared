/*
** EPITECH PROJECT, 2025
** VkWrapper-Test [WSL : Ubuntu]
** File description:
** Texture
*/

#include "Texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace ES::Plugin::Wrapper {

void Texture::Create(const std::string &texturePath)
{
    _pixels = stbi_load(texturePath.c_str(), &_width, &_height, &_channels, STBI_rgb_alpha);

    if (!_pixels)
        throw VkWrapperError("failed to load texture image (" + texturePath + "): " + stbi_failure_reason());
}

void Texture::Destroy(const VkDevice &_device)
{
    if (!_pixels)
        return;

    stbi_image_free(_pixels);
    _pixels = nullptr;
    _width = 0;
    _height = 0;
    _channels = 0;

    if (_image == VK_NULL_HANDLE || _imageMemory == VK_NULL_HANDLE || _imageView == VK_NULL_HANDLE)
        return;

    vkDestroyImageView(_device, _imageView, nullptr);
    vkDestroyImage(_device, _image, nullptr);
    vkFreeMemory(_device, _imageMemory, nullptr);
}

} // namespace ES::Plugin::Wrapper
