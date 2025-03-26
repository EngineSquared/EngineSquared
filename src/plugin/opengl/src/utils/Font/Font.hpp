#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

#include <stb_truetype.h>
#include <vector>
#include <memory>

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
    ~Font() = default;

    void RenderText(const std::string &text, float x, float y, float scale, GLuint VAO, GLuint VBO) const;

  private:
    std::unordered_map<char, Character> characters;
    std::shared_ptr<stbtt_fontinfo> fontInfo;
    std::shared_ptr<std::vector<unsigned char>> fontBuffer;

    void LoadFont(const std::string &fontPath, int fontSize);
};

} // namespace ES::Plugin::OpenGL::Utils
