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
 * @file GLSpriteBuffer.hpp
 * @brief GLSpriteBuffer class declaration.
 *
 * This class is used to manage OpenGL buffers. It is used to store the VAO,
 * and VBO. It also provides methods to draw and destroy the buffers.
 *
 * @author @EngineSquared Team
 * @version 0.0.0
 * @date 2025-04-04
 **************************************************************************/

#pragma once

#include <GL/glew.h>

#include <vector>

#include "Loader.hpp"
#include "Sprite.hpp"

namespace ES::Plugin::OpenGL::Utils {

class GLSpriteBuffer {
  public:
    GLSpriteBuffer() = default;
    GLSpriteBuffer(const GLSpriteBuffer &) = default;
    GLSpriteBuffer(GLSpriteBuffer &&) = default;
    ~GLSpriteBuffer() = default;

    GLSpriteBuffer &operator=(const GLSpriteBuffer &) = default;
    GLSpriteBuffer &operator=(GLSpriteBuffer &&) = default;

    void DestroyGLSpriteBuffers() const noexcept;

    void GenerateGLSpriteBuffers(const Component::Sprite &sprite) noexcept;

    void Update(const Component::Sprite &sprite) const noexcept;

    void Draw() const;

    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint IBO = 0;
    GLuint VBO_texCoords = 0;
};

} // namespace ES::Plugin::OpenGL::Utils
