#include "Font.hpp"
#include "Logger.hpp"
#include <array>
#include <fmt/format.h>
#include <fstream>
#include <iostream>
#include <vector>

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

namespace ES::Plugin::OpenGL::Utils {

Font::Font(const std::string &fontPath, int fontSize) { LoadFont(fontPath, fontSize); }

void Font::LoadFont(const std::string &fontPath, int fontSize)
{
    std::ifstream file(fontPath, std::ios::binary | std::ios::ate);
    if (!file.is_open())
    {
        ES::Utils::Log::Error(fmt::format("Failed to open font file: {}", fontPath));
        return;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<unsigned char> fontBuffer(size);
    file.read(reinterpret_cast<char *>(fontBuffer.data()), size);
    file.close();

    if (stbtt_InitFont(&fontInfo, fontBuffer.data(), stbtt_GetFontOffsetForIndex(fontBuffer.data(), 0)) == 0)
    {
        ES::Utils::Log::Error("Failed to initialize font");
        return;
    }

    float scale = stbtt_ScaleForPixelHeight(&fontInfo, fontSize);
    int ascent;
    int descent;
    int lineGap;
    stbtt_GetFontVMetrics(&fontInfo, &ascent, &descent, &lineGap);
    ascent = static_cast<int>(ascent * scale);
    descent = static_cast<int>(descent * scale);

    for (unsigned char c = 32; c < 128; c++)
    {
        int width;
        int height;
        int xOffset;
        int yOffset;
        unsigned char *bitmap = stbtt_GetCodepointBitmap(&fontInfo, 0, scale, c, &width, &height, &xOffset, &yOffset);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, bitmap);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        int advance;
        int lsb;
        stbtt_GetCodepointHMetrics(&fontInfo, c, &advance, &lsb);
        float scaledAdvance = (advance * scale);

        Character character;
        character.textureID = texture;
        character.size = {(float) width, (float) height};
        character.bearing = {(float) xOffset, (float) yOffset};
        character.advance = scaledAdvance;

        characters[c] = character;

        stbtt_FreeBitmap(bitmap, nullptr);
    }

    ES::Utils::Log::Info(fmt::format("Font loaded: {}", fontPath));
}

const void Font::RenderText(const std::string &text, float x, float y, float scale, GLuint VAO, GLuint VBO) const
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    for (const char &c : text)
    {
        if (!characters.contains(c))
        {
#ifdef ES_DEBUG
            ES::Utils::Log::Warn(fmt::format("Character not found: 0x{:02X}", static_cast<unsigned char>(c)));
#endif
            continue;
        }

        Character ch = characters.at(c);

        float xpos = x + ch.bearing.x * scale;
        float ypos = y - ch.size.y * scale - ch.bearing.y * scale;

        float w = ch.size.x * scale;
        float h = ch.size.y * scale;

        std::array<std::array<float, 4>, 6> vertices = {
            {{xpos, ypos + h, 0.0f, 0.0f},
             {xpos, ypos, 0.0f, 1.0f},
             {xpos + w, ypos, 1.0f, 1.0f},

             {xpos, ypos + h, 0.0f, 0.0f},
             {xpos + w, ypos, 1.0f, 1.0f},
             {xpos + w, ypos + h, 1.0f, 0.0f}}
        };

        glBindTexture(GL_TEXTURE_2D, ch.textureID);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        x += ch.advance * scale;

        // TODO: implement kerning
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_BLEND);
}

} // namespace ES::Plugin::OpenGL::Utils
