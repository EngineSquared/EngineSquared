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
    std::array<glm::vec3, 4> vertices = {   
        glm::vec3{0.f,                0.f,                0.f},
        glm::vec3{sprite.rect.size.x, 0.f,                0.f},
        glm::vec3{0.f,                sprite.rect.size.y, 0.f},
        glm::vec3{sprite.rect.size.x, sprite.rect.size.y, 0.f}
    };
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
    glEnableVertexAttribArray(0);

    // Element indices buffer
    std::array<unsigned int, 6> indices = {2, 0, 1, 2, 1, 3};
    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void GLSpriteBuffer::Draw() const
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void GLSpriteBuffer::Update(const Component::Sprite &sprite) const noexcept
{
    glBindVertexArray(VAO);

    // Vertex positions VBO
    std::array<glm::vec3, 4> vertices = {
        glm::vec3{0.f,                0.f,                0.f},
        glm::vec3{sprite.rect.size.x, 0.f,                0.f},
        glm::vec3{0.f,                sprite.rect.size.y, 0.f},
        glm::vec3{sprite.rect.size.x, sprite.rect.size.y, 0.f}
    };
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
}

} // namespace ES::Plugin::OpenGL::Utils
