#pragma once

#include <GL/glew.h>

#include "Logger.hpp"

namespace ES::Plugin::OpenGL::Utils {

class Texture {
  public:
    explicit Texture(const std::string &texturePath);
    explicit Texture(const void *rawData, int width, int height);
    Texture(int width, int height, int channels, GLuint textureID)
        : _width(width), _height(height), _channels(channels), _textureID(textureID)
    {
    }
    ~Texture();

    Texture(const Texture &) = default;
    Texture(Texture &&) = default;
    Texture &operator=(const Texture &) = default;
    Texture &operator=(Texture &&) = default;

    void Bind() const;

    [[nodiscard]] int GetWidth() const { return _width; }
    [[nodiscard]] int GetHeight() const { return _height; }
    [[nodiscard]] bool IsValid() const { return _textureID != 0; }
    [[nodiscard]] GLuint GetTexID() const { return _textureID; }

  private:
    void LoadTexture(const std::string &texturePath);
    void LoadTexture(const void *rawData);

  private:
    int _width = 0;
    int _height = 0;
    int _channels = 0;
    GLuint _textureID = 0;
};

} // namespace ES::Plugin::OpenGL::Utils
