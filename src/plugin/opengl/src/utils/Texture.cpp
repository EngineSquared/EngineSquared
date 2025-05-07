#include "Texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace ES::Plugin::OpenGL::Utils {

Texture::Texture(const std::string &texturePath) { LoadTexture(texturePath); }

Texture::~Texture()
{
    if (!_textureID)
        return;

    glDeleteTextures(1, &_textureID);
    _textureID = 0;
}

void Texture::LoadTexture(const std::string &texturePath)
{
    uint8_t *pixels = stbi_load(texturePath.c_str(), &_width, &_height, &_channels, STBI_rgb_alpha);

    if (!pixels)
    {
        ES::Utils::Log::Error("Failed to load texture image (" + texturePath + "): " + stbi_failure_reason());
        return;
    }

    glGenTextures(1, &_textureID);
    glBindTexture(GL_TEXTURE_2D, _textureID);

    GLenum format = GL_RGBA;
    GLenum internalFormat = GL_SRGB_ALPHA;
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, _width, _height, 0, format, GL_UNSIGNED_BYTE, pixels);

    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(pixels);
    ES::Utils::Log::Info(fmt::format("Texture loaded: {}", texturePath));
}

void Texture::Bind() const
{
    if (!_textureID)
        return;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _textureID);
}

} // namespace ES::Plugin::OpenGL::Utils
