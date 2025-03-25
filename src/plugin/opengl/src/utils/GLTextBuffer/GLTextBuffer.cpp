#include "GLTextBuffer.hpp"

namespace ES::Plugin::OpenGL::Utils {

void GLTextBuffer::Draw(const ES::Plugin::UI::Component::Text &text) const noexcept
{
    
}

void GLTextBuffer::DestroyGLTextBuffers() const noexcept
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void GLTextBuffer::GenerateGLTextBuffers(const ES::Plugin::UI::Component::Text &text) noexcept
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

void GLTextBuffer::Update(const ES::Plugin::UI::Component::Text &text) const noexcept
{
    
}

} // namespace ES::Plugin::OpenGL::Utils
