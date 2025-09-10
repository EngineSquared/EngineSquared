#include <cstring>
#include <memory>
#include <stb_image.h>

#include "OpenGL.pch.hpp"

#include "utils/CubeMap.hpp"

namespace ES::Plugin::OpenGL::Utils {

CubeMap::CubeMap(std::string_view cubeMapPath)
{
    if (!LoadFromCross(cubeMapPath))
    {
        ES::Utils::Log::Error(fmt::format("Failed to load cubemap from cross: {}", cubeMapPath));
    }
}

CubeMap::CubeMap(const std::array<std::string, 6> &facesPath)
{
    if (!LoadFromFaces(facesPath))
    {
        ES::Utils::Log::Error("Failed to load cubemap from faces");
    }
}

CubeMap::~CubeMap() noexcept { Cleanup(); }

CubeMap::CubeMap(CubeMap &&other) noexcept
    : _width(other._width), _height(other._height), _channels(other._channels), _textureID(other._textureID)
{
    other._textureID = 0;
}

CubeMap &CubeMap::operator=(CubeMap &&other) noexcept
{
    if (this != &other)
    {
        Cleanup();
        _width = other._width;
        _height = other._height;
        _channels = other._channels;
        _textureID = other._textureID;
        other._textureID = 0;
    }
    return *this;
}

bool CubeMap::LoadFromFaces(const std::array<std::string, 6> &faces) noexcept
{
    glGenTextures(1, &_textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _textureID);

    bool success = true;

    for (std::uint32_t i = 0; i < static_cast<std::uint32_t>(CubeFace::Count); ++i)
    {
        int width;
        int height;
        int nrChannels;

        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);

        if (!data)
        {
            ES::Utils::Log::Error(fmt::format("Cubemap texture failed to load at path: {}", faces[i]));
            success = false;
            continue;
        }

        if (_width == 0)
        {
            _width = width;
            _height = height;
            _channels = nrChannels;
        }

        const GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }

    if (success)
        SetupTextureParameters();
    else
        Cleanup();

    return success;
}

bool CubeMap::LoadFromCross(std::string_view path) noexcept
{
    int width;
    int height;
    int channels;

    struct STBIDeleter {
        void operator()(unsigned char *ptr) const noexcept
        {
            if (ptr)
                stbi_image_free(ptr);
        }
    };

    std::unique_ptr<unsigned char, STBIDeleter> data{
        stbi_load(std::string{path}.c_str(), &width, &height, &channels, 0)};

    if (!data)
    {
        ES::Utils::Log::Error(fmt::format("Failed to load cross image: {}", path));
        return false;
    }

    // Validate cross format (4:3 aspect ratio)
    if (width % 4 != 0 || height % 3 != 0 || width / 4 != height / 3)
    {
        ES::Utils::Log::Error(fmt::format("Invalid cubemap cross format (must be 4:3 aspect ratio): {}", path));
        return false;
    }

    const int faceSize = width / 4;
    _width = faceSize;
    _height = faceSize;
    _channels = channels;

    constexpr std::array<std::array<int, 2>, 6> faceOffsets{
        {
         {{2, 1}}, // right  (+X)
            {{0, 1}}, // left   (-X)
            {{1, 0}}, // top    (+Y)
            {{1, 2}}, // bottom (-Y)
            {{1, 1}}, // front  (+Z)
            {{3, 1}}  // back   (-Z)
        }
    };

    glGenTextures(1, &_textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _textureID);

    bool success = true;

    for (std::uint32_t i = 0; i < static_cast<std::uint32_t>(CubeFace::Count); ++i)
    {
        const int offsetX = faceOffsets[i][0] * faceSize;
        const int offsetY = faceOffsets[i][1] * faceSize;

        // RAII for face data
        std::string faceData;
        faceData.resize(faceSize * faceSize * channels);

        if (faceData.empty())
        {
            ES::Utils::Log::Error("Failed to allocate memory for cubemap face");
            success = false;
            break;
        }

        // Extract face data
        for (int y = 0; y < faceSize; ++y)
        {
            const auto srcOffset = ((offsetY + y) * width + offsetX) * channels;
            const auto dstOffset = y * faceSize * channels;
            std::memcpy(faceData.data() + dstOffset, data.get() + srcOffset, faceSize * channels);
        }

        const GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, faceSize, faceSize, 0, format, GL_UNSIGNED_BYTE,
                     reinterpret_cast<const unsigned char *>(faceData.data()));
    }

    if (success)
        SetupTextureParameters();
    else
        Cleanup();

    return success;
}

void CubeMap::SetupTextureParameters() noexcept
{
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void CubeMap::Bind(std::uint32_t textureUnit) const noexcept
{
    if (!IsValid())
        return;

    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _textureID);
}

void CubeMap::Cleanup() noexcept
{
    if (IsValid())
    {
        glDeleteTextures(1, &_textureID);
        _textureID = 0;
    }
}

} // namespace ES::Plugin::OpenGL::Utils
