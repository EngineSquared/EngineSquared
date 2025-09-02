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
 * @file GLTextBuffer.hpp
 * @brief GLTextBuffer class declaration.
 *
 * This class is used to manage OpenGL buffers. It is used to store the VAO,
 * and VBO. It also provides methods to draw and destroy the buffers.
 *
 * @author @EngineSquared Team
 * @version 0.0.0
 * @date 2025-03-21
 **************************************************************************/

#pragma once

#include <GL/glew.h>

#include <vector>

#include "Font.hpp"
#include "Loader.hpp"
#include "Text.hpp"

namespace ES::Plugin::OpenGL::Utils {

class GLTextBuffer {
  public:
    GLTextBuffer() = default;
    GLTextBuffer(const GLTextBuffer &) = default;
    GLTextBuffer(GLTextBuffer &&) = default;
    ~GLTextBuffer() = default;

    GLTextBuffer &operator=(const GLTextBuffer &) = default;
    GLTextBuffer &operator=(GLTextBuffer &&) = default;

    void DestroyGLTextBuffers() const noexcept;

    void GenerateGLTextBuffers() noexcept;

    void RenderText(const ES::Plugin::UI::Component::Text &text, const ES::Plugin::OpenGL::Utils::Font &font) const;

    GLuint VAO = 0;
    GLuint VBO = 0;
};

} // namespace ES::Plugin::OpenGL::Utils
