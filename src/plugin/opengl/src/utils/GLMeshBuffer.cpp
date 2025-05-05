#include "GLMeshBuffer.hpp"

namespace ES::Plugin::OpenGL::Utils {

void GLMeshBuffer::Draw(const Object::Component::Mesh &mesh) const noexcept
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, mesh.indices.size() * sizeof(uint32_t), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void GLMeshBuffer::DestroyGLMeshBuffers() const noexcept
{
    glDeleteBuffers(1, &VBO_position);
    glDeleteBuffers(1, &VBO_normal);
    glDeleteBuffers(1, &IBO);
    glDeleteVertexArrays(1, &VAO);
}

void GLMeshBuffer::GenerateGLMeshBuffers(const Object::Component::Mesh &mesh) noexcept
{
    // create vao, vbo and ibo here... (We didn't use std::vector here...)
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Vertex positions VBO
    glGenBuffers(1, &VBO_position);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_position);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(glm::vec3), mesh.vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    // Vertex Normal VBO
    glGenBuffers(1, &VBO_normal);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_normal);
    glBufferData(GL_ARRAY_BUFFER, mesh.normals.size() * sizeof(glm::vec3), mesh.normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);

    // Vertex Texture Coordinates VBO
    glGenBuffers(1, &VBO_texCoords);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_texCoords);
    glBufferData(GL_ARRAY_BUFFER, mesh.texCoords.size() * sizeof(glm::vec2), mesh.texCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(2);

    // Element indices buffer
    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(uint32_t), mesh.indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void GLMeshBuffer::Update(const Object::Component::Mesh &mesh) const noexcept
{
    glBindVertexArray(VAO);

    // Vertex positions VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO_position);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(glm::vec3), mesh.vertices.data(), GL_STATIC_DRAW);

    // Vertex Normal VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO_normal);
    glBufferData(GL_ARRAY_BUFFER, mesh.normals.size() * sizeof(glm::vec3), mesh.normals.data(), GL_STATIC_DRAW);

    // Vertex Texture Coordinates VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO_texCoords);
    glBufferData(GL_ARRAY_BUFFER, mesh.texCoords.size() * sizeof(glm::vec2), mesh.texCoords.data(), GL_STATIC_DRAW);

    // Element indices buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(uint32_t), mesh.indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
}

} // namespace ES::Plugin::OpenGL::Utils
