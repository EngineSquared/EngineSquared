#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

#include <memory>
#include <stb_truetype.h>
#include <vector>

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

    inline const Character &GetCharacter(char c) const { return characters.at(c); }

    inline bool HasCharacter(char c) const { return characters.contains(c); }

  private:
    std::unordered_map<char, Character> characters;
    std::shared_ptr<stbtt_fontinfo> fontInfo;
    std::shared_ptr<std::vector<unsigned char>> fontBuffer;

    void LoadFont(const std::string &fontPath, int fontSize);
};

} // namespace ES::Plugin::OpenGL::Utils
