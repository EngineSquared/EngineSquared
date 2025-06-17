#include "OpenGL.pch.hpp"

#include "CubeMap.hpp"

#include <stb_image.h>

namespace ES::Plugin::OpenGL::Utils {

CubeMap::CubeMap(const std::string &cubeMapPath) { loadFromCross(cubeMapPath); }
CubeMap::CubeMap(const std::array<std::string, 6> &facesPath) { loadFromFaces(facesPath); }

CubeMap::~CubeMap()
{
    if (!_textureID)
        return;

    glDeleteTextures(1, &_textureID);
    _textureID = 0;
}

void CubeMap::loadFromFaces(const std::array<std::string, 6> &faces)
{
    glGenTextures(1, &_textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _textureID);

    int width, height, nrChannels;
    for (GLuint i = 0; i < 6; i++)
    {
        unsigned char *data = stbi_load(faces[i], &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE,
                         data);
            stbi_image_free(data);
        }
        else
        {
            fprintf(stderr, "Cubemap texture failed to load at path: %s\n", faces[i]);
            stbi_image_free(data);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void CubeMap::loadFromCross(const std::string &path)
{
    int width, height, channels;
    unsigned char *data = stbi_load(path, &width, &height, &channels, 0);
    if (!data)
    {
        fprintf(stderr, "Failed to load image: %s\n", path);
        return 0;
    }

    if (width % 4 != 0 || height % 3 != 0 || width / 4 != height / 3)
    {
        fprintf(stderr, "Invalid cubemap cross format (must be 4:3 aspect ratio).\n");
        stbi_image_free(data);
        return 0;
    }

    int faceSize = width / 4;

    // Face order: left, front, right, back, top, bottom
    int faceOffsets[6][2] = {
        {0, 1}, // left
        {1, 1}, // front
        {2, 1}, // right
        {3, 1}, // back
        {1, 0}, // top
        {1, 2}  // bottom
    };

    glGenTextures(1, &_textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _textureID);

    for (GLuint i = 0; i < 6; ++i)
    {
        int offsetX = faceOffsets[i][0] * faceSize;
        int offsetY = faceOffsets[i][1] * faceSize;

        // stbi loads image data from top-left, OpenGL expects bottom-left
        unsigned char *faceData = malloc(faceSize * faceSize * channels);
        if (!faceData)
        {
            fprintf(stderr, "Failed to allocate memory for cubemap face.\n");
            stbi_image_free(data);
            return 0;
        }

        for (int y = 0; y < faceSize; ++y)
        {
            memcpy(faceData + y * faceSize * channels, data + ((offsetY + y) * width + offsetX) * channels,
                    faceSize * channels);
        }

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, channels == 4 ? GL_RGBA : GL_RGB, faceSize, faceSize, 0,
                        channels == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, faceData);

        free(faceData);
    }

    stbi_image_free(data);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void CubeMap::Bind() const
{
    if (!_textureID)
        return;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _textureID);
}

} // namespace ES::Plugin::OpenGL::Utils
