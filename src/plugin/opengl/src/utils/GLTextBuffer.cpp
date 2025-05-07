#include "GLTextBuffer.hpp"

namespace ES::Plugin::OpenGL::Utils {

void GLTextBuffer::DestroyGLTextBuffers() const noexcept
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void GLTextBuffer::GenerateGLTextBuffers() noexcept
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void GLTextBuffer::RenderText(const ES::Plugin::UI::Component::Text &text,
                              const ES::Plugin::OpenGL::Utils::Font &font) const
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    float x = text.position.x;
    float y = text.position.y;

    for (const char &c : text.text)
    {
        if (!font.HasCharacter(c))
        {
            ES::Utils::Log::Warn(fmt::format("Character not found: 0x{:02X}", static_cast<unsigned char>(c)));
            continue;
        }

        const auto &ch = font.GetCharacter(c);

        float xpos = x + ch.bearing.x * text.scale;
        float ypos = y - ch.size.y * text.scale - ch.bearing.y * text.scale;

        float w = ch.size.x * text.scale;
        float h = ch.size.y * text.scale;

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

        x += ch.advance * text.scale;
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_BLEND);
}

} // namespace ES::Plugin::OpenGL::Utils
