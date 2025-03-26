#include "Font.hpp"
#include "Logger.hpp"
#include <array>
#include <fmt/format.h>
#include <fstream>
#include <iostream>

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

namespace ES::Plugin::OpenGL::Utils {

Font::Font(const std::string &fontPath, int fontSize) { LoadFont(fontPath, fontSize); }

void Font::LoadFont(const std::string &fontPath, int fontSize)
{
    fontBuffer = std::make_shared<std::vector<unsigned char>>();
    fontInfo = std::make_shared<stbtt_fontinfo>();

    std::ifstream file(fontPath, std::ios::binary | std::ios::ate);
    if (!file.is_open())
    {
        ES::Utils::Log::Error(fmt::format("Failed to open font file: {}", fontPath));
        return;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    fontBuffer->resize(size);
    file.read(reinterpret_cast<char *>(fontBuffer->data()), size);
    file.close();

    if (stbtt_InitFont(fontInfo.get(), fontBuffer->data(), stbtt_GetFontOffsetForIndex(fontBuffer->data(), 0)) == 0)
    {
        ES::Utils::Log::Error("Failed to initialize font");
        return;
    }

    float scale = stbtt_ScaleForPixelHeight(fontInfo.get(), fontSize);
    int ascent;
    int descent;
    int lineGap;
    stbtt_GetFontVMetrics(fontInfo.get(), &ascent, &descent, &lineGap);
    ascent = static_cast<int>(ascent * scale);
    descent = static_cast<int>(descent * scale);

    for (unsigned char c = 32; c < 128; c++)
    {
        int width;
        int height;
        int xOffset;
        int yOffset;
        unsigned char *bitmap =
            stbtt_GetCodepointBitmap(fontInfo.get(), 0, scale, c, &width, &height, &xOffset, &yOffset);

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
        stbtt_GetCodepointHMetrics(fontInfo.get(), c, &advance, &lsb);
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

} // namespace ES::Plugin::OpenGL::Utils
