#include "GLSpriteBuffer.hpp"

namespace ES::Plugin::OpenGL::Utils {

void GLSpriteBuffer::DestroyGLSpriteBuffers() const noexcept
{
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &IBO);
    glDeleteVertexArrays(1, &VAO);
}

void GLSpriteBuffer::GenerateGLSpriteBuffers(const Component::Sprite &sprite) noexcept
{
    // create vao, vbo and ibo here... (We didn't use std::vector here...)
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Vertex positions VBO
    glm::vec3 vertices[4] = {
        {0.f, 0.f, 0.f},
        {sprite.rect.size.x, 0.f, 0.f},
        {0.f, sprite.rect.size.y, 0.f},
        {sprite.rect.size.x, sprite.rect.size.y, 0.f}
    };
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
    glEnableVertexAttribArray(0);  

    // Element indices buffer
    unsigned int indices[6] = {
        2, 0, 1,
        2, 1, 3
    };
    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void GLSpriteBuffer::Draw(const Component::Sprite &sprite) const
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void GLSpriteBuffer::Update(const Component::Sprite &sprite) const noexcept
{
    glBindVertexArray(VAO);

    // Vertex positions VBO
    glm::vec3 vertices[4] = {
        {0.f, 0.f, 0.f},
        {sprite.rect.size.x, 0.f, 0.f},
        {0.f, sprite.rect.size.y, 0.f},
        {sprite.rect.size.x, sprite.rect.size.y, 0.f}
    };
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(0);
}

} // namespace ES::Plugin::OpenGL::Utils
