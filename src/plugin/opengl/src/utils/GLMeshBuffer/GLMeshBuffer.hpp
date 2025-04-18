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
 * @file GLMeshBuffer.hpp
 * @brief GLMeshBuffer class declaration.
 *
 * This class is used to manage OpenGL buffers. It is used to store the VAO,
 * VBOs and IBOs. It also provides methods to draw and destroy the buffers.
 *
 * @author @EngineSquared Team
 * @version 0.0.0
 * @date 2025-03-21
 **************************************************************************/

#pragma once

#include <GL/glew.h>

#include <vector>

#include "Loader.hpp"

#include "Mesh.hpp"

namespace ES::Plugin::OpenGL::Utils {

class GLMeshBuffer {
  public:
    GLMeshBuffer() = default;
    GLMeshBuffer(const GLMeshBuffer &) = default;
    GLMeshBuffer(GLMeshBuffer &&) = default;
    ~GLMeshBuffer() = default;

    GLMeshBuffer &operator=(const GLMeshBuffer &) = default;
    GLMeshBuffer &operator=(GLMeshBuffer &&) = default;

    void Draw(const Object::Component::Mesh &mesh) const noexcept;

    void DestroyGLMeshBuffers() const noexcept;

    void GenerateGLMeshBuffers(const Object::Component::Mesh &mesh) noexcept;

    void Update(const Object::Component::Mesh &mesh) const noexcept;

    GLuint VAO = 0;
    GLuint VBO_position = 0;
    GLuint VBO_normal = 0;
    GLuint VBO_texCoords = 0;
    GLuint IBO = 0;
};

} // namespace ES::Plugin::OpenGL::Utils
