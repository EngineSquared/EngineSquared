#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

#include <stb_truetype.h>

namespace ES::Plugin::OpenGL::Utils {

struct Character {
    GLuint textureID;
    glm::ivec2 size;
    glm::ivec2 bearing;
    GLuint advance;
};

class Font {
  public:
    explicit Font(const std::string &fontPath, int fontSize);
    ~Font();

    const void RenderText(const std::string &text, float x, float y, float scale, const glm::vec3 &color, GLuint VAO,
                          GLuint VBO) const;

  private:
    std::unordered_map<char, Character> characters;
    stbtt_fontinfo fontInfo;
    unsigned char *fontBuffer;
    unsigned char *bitmap;

    void LoadFont(const std::string &fontPath, int fontSize);
};

} // namespace ES::Plugin::OpenGL::Utils
