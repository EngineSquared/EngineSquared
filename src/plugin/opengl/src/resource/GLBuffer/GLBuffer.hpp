/**************************************************************************
 * Engine² v0.0.0
 *
 * This file is part of the Engine² project that is under GPL-3.0 License.
 * Copyright © 2024 by @EngineSquared, All rights reserved.
 *
 * Engine² is a free software: you can redistribute it and/or modify
 * it under the terms of the GPL-3.0 License as published by the
 * Free Software Foundation. See the GPL-3.0 License for more details.
 *
 * @file GLBuffer.hpp
 * @brief GLBuffer class declaration.
 *
 * This class is used to manage OpenGL buffers. It is used to store the VAO,
 * VBOs and IBOs. It also provides methods to draw and destroy the buffers.
 *
 * @author @EngineSquared Team
 * @version 0.0.0
 * @date 2025-03-21
 **************************************************************************/

#ifndef GLBUFFER_HPP_
#define GLBUFFER_HPP_

#include <GL/glew.h>

#include <vector>

#include "Loader.hpp"
#include "Material.hpp"

#include "component/Mesh.hpp"

namespace ES::Plugin::OpenGL::Resource {

class GLBuffer {
  public:
    GLBuffer() = default;
    GLBuffer(const GLBuffer &) = default;
    GLBuffer(GLBuffer &&) = default;
    ~GLBuffer() = default;

    GLBuffer &operator=(const GLBuffer &) = default;
    GLBuffer &operator=(GLBuffer &&) = default;

    void draw(const Object::Component::Mesh &mesh) const
    {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, mesh.indices.size() * sizeof(uint32_t), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

    void destroyGlBuffers() const
    {
        glDeleteBuffers(1, &VBO_position);
        glDeleteBuffers(1, &VBO_normal);
        glDeleteBuffers(1, &IBO);
        glDeleteVertexArrays(1, &VAO);
    }

    void generateGlBuffers(const Object::Component::Mesh &mesh)
    {
        // create vao, vbo and ibo here... (We didn't use std::vector here...)
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        // Vertex positions VBO
        glGenBuffers(1, &VBO_position);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_position);
        glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(glm::vec3), mesh.vertices.data(),
                     GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(0);

        // Vertex Normal VBO
        glGenBuffers(1, &VBO_normal);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_normal);
        glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(glm::vec3), mesh.normals.data(),
                     GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(1);

        // Element indices buffer
        glGenBuffers(1, &IBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(uint32_t), mesh.indices.data(),
                     GL_STATIC_DRAW);

        glBindVertexArray(0);
    }

    void update(const Object::Component::Mesh &mesh) const
    {
        glBindVertexArray(VAO);

        // Vertex positions VBO
        glBindBuffer(GL_ARRAY_BUFFER, VBO_position);
        glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(glm::vec3), mesh.vertices.data(),
                     GL_STATIC_DRAW);

        // Vertex Normal VBO
        glBindBuffer(GL_ARRAY_BUFFER, VBO_normal);
        glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(glm::vec3), mesh.normals.data(),
                     GL_STATIC_DRAW);

        // Element indices buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(uint32_t), mesh.indices.data(),
                     GL_STATIC_DRAW);

        glBindVertexArray(0);
    }

    GLuint VAO;
    GLuint VBO_position;
    GLuint VBO_normal;
    GLuint IBO;
};

} // namespace ES::Plugin::OpenGL::Resource

#endif /* !GLBUFFER_HPP_ */
