#pragma once

#include <GL/glew.h>

#include "Logger.hpp"

namespace ES::Plugin::OpenGL::Utils {

class Texture {
  public:
    explicit Texture(const std::string &texturePath);
    ~Texture();

    void Bind() const;

    [[nodiscard]] int GetWidth() const { return _width; }
    [[nodiscard]] int GetHeight() const { return _height; }

  private:
    void LoadTexture(const std::string &texturePath);

  private:
    int _width = 0;
    int _height = 0;
    int _channels = 0;
    GLuint _textureID = 0;
};

} // namespace ES::Plugin::OpenGL::Utils
