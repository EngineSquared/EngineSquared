#include "Backend.hpp"

static void CheckGLError(const std::string &operation_name)
{
    GLenum errCode = glGetError();

    if (errCode != GL_NO_ERROR)
    {
        const Rml::Pair<GLenum, std::string> errNames[] = {
            {GL_INVALID_ENUM,      "GL_INVALID_ENUM"     },
            {GL_INVALID_VALUE,     "GL_INVALID_VALUE"    },
            {GL_INVALID_OPERATION, "GL_INVALID_OPERATION"},
            {GL_OUT_OF_MEMORY,     "GL_OUT_OF_MEMORY"    }
        };
        std::string message = "''";
        for (auto &err : errNames)
        {
            if (err.first == errCode)
            {
                message = err.second;
                break;
            }
        }
        ES::Utils::Log::Error(
            fmt::format("RmlUi: OpenGL error during {}. Error code 0x{:x} {}.", operation_name, errCode, message));
    }
}

ES::Plugin::UI::Utils::RenderInterface::RenderInterface(ES::Engine::Core &core) : _core(core){};

ES::Plugin::UI::Utils::RenderInterface::~RenderInterface(){};

void ES::Plugin::UI::Utils::RenderInterface::UseShaderProgram(const entt::hashed_string &program_id)
{
    auto &shaderManager = _core.GetResource<ES::Plugin::OpenGL::Resource::ShaderManager>();

    if (program_id == entt::hashed_string(""))
    {
        if (activeShaderProgram != entt::hashed_string(""))
            shaderManager.Get(activeShaderProgram).Disable();
        return;
    }

    if (activeShaderProgram != program_id)
    {
        if (activeShaderProgram != entt::hashed_string(""))
            shaderManager.Get(activeShaderProgram).Disable();
        activeShaderProgram = program_id;
    }
    shaderManager.Get(program_id).Use();
}

void ES::Plugin::UI::Utils::RenderInterface::DisableActiveShaderProgram()
{
    auto &shaderManager = _core.GetResource<ES::Plugin::OpenGL::Resource::ShaderManager>();
    if (activeShaderProgram != entt::hashed_string(""))
    {
        shaderManager.Get(activeShaderProgram).Disable();
        activeShaderProgram = entt::hashed_string("");
    }
}

Rml::Rectanglei ES::Plugin::UI::Utils::RenderInterface::VerticallyFlipped(Rml::Rectanglei rect, int viewport_height)
{
    RMLUI_ASSERT(rect.Valid());
    Rml::Rectanglei flipped_rect = rect;
    flipped_rect.p0.y = viewport_height - rect.p1.y;
    flipped_rect.p1.y = viewport_height - rect.p0.y;
    return flipped_rect;
}

bool ES::Plugin::UI::Utils::RenderInterface::CreateFramebuffer(FramebufferData &out_fb, int width, int height,
                                                               int samples, FramebufferAttachment attachment,
                                                               GLuint shared_depth_stencil_buffer)
{
#ifdef RMLUI_PLATFORM_EMSCRIPTEN
    constexpr GLint wrap_mode = GL_CLAMP_TO_EDGE;
#else
    constexpr GLint wrap_mode = GL_CLAMP_TO_BORDER; // GL_REPEAT GL_MIRRORED_REPEAT GL_CLAMP_TO_EDGE
#endif

    constexpr GLenum color_format = GL_RGBA8;   // GL_RGBA8 GL_SRGB8_ALPHA8 GL_RGBA16F
    constexpr GLint min_mag_filter = GL_LINEAR; // GL_NEAREST
    const Rml::Colourf border_color(0.f, 0.f);

    GLuint framebuffer = 0;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    GLuint color_tex_buffer = 0;
    GLuint color_render_buffer = 0;
    if (samples > 0)
    {
        glGenRenderbuffers(1, &color_render_buffer);
        glBindRenderbuffer(GL_RENDERBUFFER, color_render_buffer);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, color_format, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, color_render_buffer);
    }
    else
    {
        glGenTextures(1, &color_tex_buffer);
        glBindTexture(GL_TEXTURE_2D, color_tex_buffer);
        glTexImage2D(GL_TEXTURE_2D, 0, color_format, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_mag_filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, min_mag_filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_mode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_mode);
#ifndef RMLUI_PLATFORM_EMSCRIPTEN
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &border_color[0]);
#endif

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_tex_buffer, 0);
    }

    // Create depth/stencil buffer storage attachment.
    GLuint depth_stencil_buffer = 0;
    if (attachment != FramebufferAttachment::None)
    {
        if (shared_depth_stencil_buffer)
        {
            // Share depth/stencil buffer
            depth_stencil_buffer = shared_depth_stencil_buffer;
        }
        else
        {
            // Create new depth/stencil buffer
            glGenRenderbuffers(1, &depth_stencil_buffer);
            glBindRenderbuffer(GL_RENDERBUFFER, depth_stencil_buffer);

            glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, width, height);
        }

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth_stencil_buffer);
    }

    const GLuint framebuffer_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (framebuffer_status != GL_FRAMEBUFFER_COMPLETE)
    {
        ES::Utils::Log::Error(
            fmt::format("OpenGL framebuffer could not be generated. Error code {:x}.", framebuffer_status));
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    out_fb = {};
    out_fb.width = width;
    out_fb.height = height;
    out_fb.framebuffer = framebuffer;
    out_fb.color_tex_buffer = color_tex_buffer;
    out_fb.color_render_buffer = color_render_buffer;
    out_fb.depth_stencil_buffer = depth_stencil_buffer;
    out_fb.owns_depth_stencil_buffer = !shared_depth_stencil_buffer;

    CheckGLError("CreateFramebuffer");
    return true;
}

void ES::Plugin::UI::Utils::RenderInterface::DestroyFramebuffer(FramebufferData &fb)
{
    if (fb.framebuffer)
        glDeleteFramebuffers(1, &fb.framebuffer);
    if (fb.color_tex_buffer)
        glDeleteTextures(1, &fb.color_tex_buffer);
    if (fb.color_render_buffer)
        glDeleteRenderbuffers(1, &fb.color_render_buffer);
    if (fb.owns_depth_stencil_buffer && fb.depth_stencil_buffer)
        glDeleteRenderbuffers(1, &fb.depth_stencil_buffer);
    fb = {};
    CheckGLError("DestroyFrameBuffer");
}

Rml::CompiledGeometryHandle
ES::Plugin::UI::Utils::RenderInterface::CompileGeometry(Rml::Span<const Rml::Vertex> vertices,
                                                        Rml::Span<const int> indices)
{
    constexpr GLenum draw_usage = GL_STATIC_DRAW;

    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ibo = 0;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ibo);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Rml::Vertex) * vertices.size(),
                 reinterpret_cast<const void *>(vertices.data()), draw_usage);

    glEnableVertexAttribArray(static_cast<GLuint>(VertexAttribute::Position));
    glVertexAttribPointer(static_cast<GLuint>(VertexAttribute::Position), 2, GL_FLOAT, GL_FALSE, sizeof(Rml::Vertex),
                          reinterpret_cast<const GLvoid *>(offsetof(Rml::Vertex, position)));

    glEnableVertexAttribArray(static_cast<GLuint>(VertexAttribute::Color0));
    glVertexAttribPointer(static_cast<GLuint>(VertexAttribute::Color0), 4, GL_UNSIGNED_BYTE, GL_TRUE,
                          sizeof(Rml::Vertex), reinterpret_cast<const GLvoid *>(offsetof(Rml::Vertex, colour)));

    glEnableVertexAttribArray(static_cast<GLuint>(VertexAttribute::TexCoord0));
    glVertexAttribPointer(static_cast<GLuint>(VertexAttribute::TexCoord0), 2, GL_FLOAT, GL_FALSE, sizeof(Rml::Vertex),
                          reinterpret_cast<const GLvoid *>(offsetof(Rml::Vertex, tex_coord)));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indices.size(), reinterpret_cast<const void *>(indices.data()),
                 draw_usage);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    CheckGLError("CompileGeometry");

    std::string key = "rml_mesh_" + std::to_string(_next_geom_id);
    CompiledGeometryData geometry;
    geometry.vao = vao;
    geometry.vbo = vbo;
    geometry.ibo = ibo;
    geometry.draw_count = (GLsizei) indices.size();

    Rml::CompiledGeometryHandle id = _next_geom_id;
    _geometries.emplace(id, std::move(geometry));
    _next_geom_id += 1;
    return id;
}

void ES::Plugin::UI::Utils::RenderInterface::RenderGeometry(Rml::CompiledGeometryHandle handle,
                                                            Rml::Vector2f translation,
                                                            Rml::TextureHandle texture_handle)
{
    const auto &viewportSize = _core.GetResource<ES::Plugin::OpenGL::Resource::Camera>().size;
    auto &shaderManager = _core.GetResource<ES::Plugin::OpenGL::Resource::ShaderManager>();
    const auto &texIt = _textures.find(texture_handle);
    const auto it = _geometries.find(handle);

    if (it == _geometries.end())
    {
        ES::Utils::Log::Error("RmlUi: Invalid geometry to render, it does not exist.");
        return;
    }

    glm::mat4 projection = glm::ortho(0.0f, viewportSize.x, viewportSize.y, 0.0f, -1.0f, 1.0f);

    if (texture_handle)
    {
        UseShaderProgram("RmlVertexTexture");
        if (texture_handle != TextureEnableWithoutBinding)
        {
            if (texIt != _textures.end())
            {
                auto &tex = _core.GetResource<ES::Plugin::OpenGL::Resource::TextureManager>().Get(
                    entt::hashed_string{texIt->second.key.c_str()});
                glBindTexture(GL_TEXTURE_2D, tex.GetTexID());
                texIt->second.translation = translation;
            }
            else
            {
                ES::Utils::Log::Error(fmt::format("RmlUi: Texture handle {} not found", texture_handle));
            }
            auto &texShaderProg = shaderManager.Get("RmlVertexTexture");
            glUniform1i(texShaderProg.GetUniform("_tex"), 0);
            glUniform2f(texShaderProg.GetUniform("_translate"), translation.x, translation.y);
            glUniformMatrix4fv(texShaderProg.GetUniform("_transform"), 1, GL_FALSE, glm::value_ptr(projection));
        }
    }
    else
    {
        UseShaderProgram("RmlVertexColor");
        glBindTexture(GL_TEXTURE_2D, 0);
        auto &vertColShaderProg = shaderManager.Get("RmlVertexColor");
        glUniform2f(vertColShaderProg.GetUniform("_translate"), 0.0f, 0.0f);
        glUniformMatrix4fv(vertColShaderProg.GetUniform("_transform"), 1, GL_FALSE, glm::value_ptr(projection));
    }

    glBindVertexArray(it->second.vao);
    glDrawElements(GL_TRIANGLES, it->second.draw_count, GL_UNSIGNED_INT, reinterpret_cast<const GLvoid *>(0));

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    CheckGLError("RenderCompileGeometry");
}

void ES::Plugin::UI::Utils::RenderInterface::ReleaseGeometry(Rml::CompiledGeometryHandle handle)
{
    auto it = _geometries.find(handle);
    if (it != _geometries.end())
    {
        _geometries.erase(it);
    }
}

Rml::TextureHandle ES::Plugin::UI::Utils::RenderInterface::LoadTexture(Rml::Vector2i &texture_dimensions,
                                                                       const Rml::String &source)
{
    std::string key = "rml_texture_" + std::to_string(_next_tex_id);
    const entt::hashed_string handle = entt::hashed_string{key.c_str()};

    auto &textureManager = _core.GetResource<ES::Plugin::OpenGL::Resource::TextureManager>();

    if (!textureManager.Contains(handle))
    {
        textureManager.Add(handle, source.data());
    }

    auto &texture = textureManager.Get(handle);
    if (!texture.IsValid())
    {
        ES::Utils::Log::Error(fmt::format("RmlUi: Loaded texture {} is not valid", handle.data()));
        return 0;
    }

    texture_dimensions = {texture.GetWidth(), texture.GetHeight()};

    Rml::TextureHandle tex_handle = static_cast<Rml::TextureHandle>(texture.GetTexID());
    _textures[tex_handle].key = key;
    _textures[tex_handle].size = texture_dimensions;
    _textures[tex_handle].translation = Rml::Vector2f(0.0f);
    _next_tex_id += 1;
    return tex_handle;
}

Rml::TextureHandle ES::Plugin::UI::Utils::RenderInterface::GenerateTexture(Rml::Span<const Rml::byte> source,
                                                                           Rml::Vector2i dimensions)
{
    RMLUI_ASSERT(source.data() && source.size() == size_t(dimensions.x * dimensions.y * 4));

    std::string key = "rml_raw_texture_" + std::to_string(_next_tex_id);
    const entt::hashed_string handle = entt::hashed_string{key.c_str()};
    auto &textureManager = _core.GetResource<ES::Plugin::OpenGL::Resource::TextureManager>();

    Rml::TextureHandle texture_id = CreateTexture(source, dimensions);
    if (texture_id == 0)
    {
        ES::Utils::Log::Error("RmlUi: Failed to create the texture from raw data");
        return {};
    }
    if (!textureManager.Contains(handle))
    {
        textureManager.Add(handle, source.data(), dimensions.x, dimensions.y);
        auto &texture = textureManager.Get(handle);
        if (!texture.IsValid())
        {
            ES::Utils::Log::Error(fmt::format("RmlUi: Loaded texture {} is not valid", handle.data()));
            return 0;
        }
    }

    _textures[texture_id].key = key;
    _textures[texture_id].size = dimensions;
    _textures[texture_id].translation = Rml::Vector2f(0.0f);
    _next_tex_id += 1;
    return texture_id;
}

Rml::TextureHandle ES::Plugin::UI::Utils::RenderInterface::CreateTexture(Rml::Span<const Rml::byte> source_data,
                                                                         Rml::Vector2i source_dimensions)
{
    GLuint texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, source_dimensions.x, source_dimensions.y, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 source_data.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, 0);

    CheckGLError("CreateTexture");

    return static_cast<Rml::TextureHandle>(texture_id);
}

void ES::Plugin::UI::Utils::RenderInterface::ReleaseTexture(Rml::TextureHandle handle)
{
    auto it = _textures.find(handle);
    if (it != _textures.end())
    {
        auto &textureManager = _core.GetResource<ES::Plugin::OpenGL::Resource::TextureManager>();
        textureManager.Remove(entt::hashed_string{it->second.key.c_str()});
        _textures.erase(it);
    }
}

void ES::Plugin::UI::Utils::RenderInterface::EnableScissorRegion(bool enable)
{
    if (enable)
        SetScissor(Rml::Rectanglei::MakeInvalid(), false);
}

void ES::Plugin::UI::Utils::RenderInterface::SetScissorRegion(Rml::Rectanglei region) { SetScissor(region, true); }

void ES::Plugin::UI::Utils::RenderInterface::SetScissor(Rml::Rectanglei region, bool vertically_flip)
{
    const auto &windowSize = _core.GetResource<ES::Plugin::Window::Resource::Window>().GetSize();

    if (region.Valid() != _scissor_state.Valid())
    {
        if (region.Valid())
            glEnable(GL_SCISSOR_TEST);
        else
            glDisable(GL_SCISSOR_TEST);
    }

    if (region.Valid() && vertically_flip)
        region = VerticallyFlipped(region, windowSize.y);

    if (region.Valid() && region != _scissor_state)
    {
        // Some render APIs don't like offscreen positions (WebGL in particular), so clamp them to the viewport.
        const int x = Rml::Math::Clamp(region.Left(), 0, windowSize.x);
        const int y = Rml::Math::Clamp(windowSize.y - region.Bottom(), 0, windowSize.y);

        glScissor(x, y, region.Width(), region.Height());
    }

    _scissor_state = region;
    CheckGLError("SetSissor");
}

void ES::Plugin::UI::Utils::RenderInterface::BeginFrame()
{
    const auto &windowSize = _core.GetResource<ES::Plugin::Window::Resource::Window>().GetSize();
    RMLUI_ASSERT(windowSize.x >= 1 && windowSize.y >= 1);

    // Backup GL state.
    _glstate_backup.enable_cull_face = glIsEnabled(GL_CULL_FACE);
    _glstate_backup.enable_blend = glIsEnabled(GL_BLEND);
    _glstate_backup.enable_stencil_test = glIsEnabled(GL_STENCIL_TEST);
    _glstate_backup.enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);
    _glstate_backup.enable_depth_test = glIsEnabled(GL_DEPTH_TEST);

    glGetIntegerv(GL_VIEWPORT, _glstate_backup.viewport);
    glGetIntegerv(GL_SCISSOR_BOX, _glstate_backup.scissor);

    glGetIntegerv(GL_ACTIVE_TEXTURE, &_glstate_backup.active_texture);

    glGetIntegerv(GL_STENCIL_CLEAR_VALUE, &_glstate_backup.stencil_clear_value);
    glGetFloatv(GL_COLOR_CLEAR_VALUE, _glstate_backup.color_clear_value);
    glGetBooleanv(GL_COLOR_WRITEMASK, _glstate_backup.color_writemask);

    glGetIntegerv(GL_BLEND_EQUATION_RGB, &_glstate_backup.blend_equation_rgb);
    glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &_glstate_backup.blend_equation_alpha);
    glGetIntegerv(GL_BLEND_SRC_RGB, &_glstate_backup.blend_src_rgb);
    glGetIntegerv(GL_BLEND_DST_RGB, &_glstate_backup.blend_dst_rgb);
    glGetIntegerv(GL_BLEND_SRC_ALPHA, &_glstate_backup.blend_src_alpha);
    glGetIntegerv(GL_BLEND_DST_ALPHA, &_glstate_backup.blend_dst_alpha);

    glGetIntegerv(GL_STENCIL_FUNC, &_glstate_backup.stencil_front.func);
    glGetIntegerv(GL_STENCIL_REF, &_glstate_backup.stencil_front.ref);
    glGetIntegerv(GL_STENCIL_VALUE_MASK, &_glstate_backup.stencil_front.value_mask);
    glGetIntegerv(GL_STENCIL_WRITEMASK, &_glstate_backup.stencil_front.writemask);
    glGetIntegerv(GL_STENCIL_FAIL, &_glstate_backup.stencil_front.fail);
    glGetIntegerv(GL_STENCIL_PASS_DEPTH_FAIL, &_glstate_backup.stencil_front.pass_depth_fail);
    glGetIntegerv(GL_STENCIL_PASS_DEPTH_PASS, &_glstate_backup.stencil_front.pass_depth_pass);

    glGetIntegerv(GL_STENCIL_BACK_FUNC, &_glstate_backup.stencil_back.func);
    glGetIntegerv(GL_STENCIL_BACK_REF, &_glstate_backup.stencil_back.ref);
    glGetIntegerv(GL_STENCIL_BACK_VALUE_MASK, &_glstate_backup.stencil_back.value_mask);
    glGetIntegerv(GL_STENCIL_BACK_WRITEMASK, &_glstate_backup.stencil_back.writemask);
    glGetIntegerv(GL_STENCIL_BACK_FAIL, &_glstate_backup.stencil_back.fail);
    glGetIntegerv(GL_STENCIL_BACK_PASS_DEPTH_FAIL, &_glstate_backup.stencil_back.pass_depth_fail);
    glGetIntegerv(GL_STENCIL_BACK_PASS_DEPTH_PASS, &_glstate_backup.stencil_back.pass_depth_pass);

    // Setup expected GL state.
    glViewport(0, 0, windowSize.x, windowSize.y);

    glClearStencil(0);
    glClearColor(0, 0, 0, 0);

    glActiveTexture(GL_TEXTURE0);

    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_CULL_FACE);

    // Set blending function for premultiplied alpha.
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

#ifndef RMLUI_PLATFORM_EMSCRIPTEN
    // We do blending in nonlinear sRGB space because that is the common practice and gives results that we are used
    // to.
    glDisable(GL_FRAMEBUFFER_SRGB);
#endif

    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 1, GLuint(-1));
    glStencilMask(GLuint(-1));
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

    glDisable(GL_DEPTH_TEST);

    SetTransform(nullptr);

    _render_layers.BeginFrame(windowSize.x, windowSize.y);
    glBindFramebuffer(GL_FRAMEBUFFER, _render_layers.GetTopLayer().framebuffer);
    glClear(GL_COLOR_BUFFER_BIT);

    UseShaderProgram("");
    _scissor_state = Rml::Rectanglei::MakeInvalid();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    CheckGLError("BeginFrame");
}

void ES::Plugin::UI::Utils::RenderInterface::EndFrame(ES::Engine::Core &)
{
    const FramebufferData &fb_active = _render_layers.GetTopLayer();
    const FramebufferData &fb_postprocess = _render_layers.GetPostprocessPrimary();
    const auto &windowSize = _core.GetResource<ES::Plugin::Window::Resource::Window>().GetSize();

    // Resolve MSAA to postprocess framebuffer.
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fb_active.framebuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb_postprocess.framebuffer);

    glBlitFramebuffer(0, 0, fb_active.width, fb_active.height, 0, 0, fb_postprocess.width, fb_postprocess.height,
                      GL_COLOR_BUFFER_BIT, GL_NEAREST);

    // Draw to backbuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, windowSize.x, windowSize.y);

    // Assuming we have an opaque background, we can just write to it with the premultiplied alpha blend mode and
    // we'll get the correct result. Instead, if we had a transparent destination that didn't use premultiplied
    // alpha, we would need to perform a manual un-premultiplication step.
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fb_postprocess.color_tex_buffer);

    _render_layers.EndFrame();

    // Restore GL state.
    if (_glstate_backup.enable_cull_face)
        glEnable(GL_CULL_FACE);
    else
        glDisable(GL_CULL_FACE);

    if (_glstate_backup.enable_blend)
        glEnable(GL_BLEND);
    else
        glDisable(GL_BLEND);

    if (_glstate_backup.enable_stencil_test)
        glEnable(GL_STENCIL_TEST);
    else
        glDisable(GL_STENCIL_TEST);

    if (_glstate_backup.enable_scissor_test)
        glEnable(GL_SCISSOR_TEST);
    else
        glDisable(GL_SCISSOR_TEST);

    if (_glstate_backup.enable_depth_test)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);

    glViewport(_glstate_backup.viewport[0], _glstate_backup.viewport[1], _glstate_backup.viewport[2],
               _glstate_backup.viewport[3]);
    glScissor(_glstate_backup.scissor[0], _glstate_backup.scissor[1], _glstate_backup.scissor[2],
              _glstate_backup.scissor[3]);

    glActiveTexture(_glstate_backup.active_texture);

    glClearStencil(_glstate_backup.stencil_clear_value);
    glClearColor(_glstate_backup.color_clear_value[0], _glstate_backup.color_clear_value[1],
                 _glstate_backup.color_clear_value[2], _glstate_backup.color_clear_value[3]);
    glColorMask(_glstate_backup.color_writemask[0], _glstate_backup.color_writemask[1],
                _glstate_backup.color_writemask[2], _glstate_backup.color_writemask[3]);

    glBlendEquationSeparate(_glstate_backup.blend_equation_rgb, _glstate_backup.blend_equation_alpha);
    glBlendFuncSeparate(_glstate_backup.blend_src_rgb, _glstate_backup.blend_dst_rgb, _glstate_backup.blend_src_alpha,
                        _glstate_backup.blend_dst_alpha);

    glStencilFuncSeparate(GL_FRONT, _glstate_backup.stencil_front.func, _glstate_backup.stencil_front.ref,
                          _glstate_backup.stencil_front.value_mask);
    glStencilMaskSeparate(GL_FRONT, _glstate_backup.stencil_front.writemask);
    glStencilOpSeparate(GL_FRONT, _glstate_backup.stencil_front.fail, _glstate_backup.stencil_front.pass_depth_fail,
                        _glstate_backup.stencil_front.pass_depth_pass);

    glStencilFuncSeparate(GL_BACK, _glstate_backup.stencil_back.func, _glstate_backup.stencil_back.ref,
                          _glstate_backup.stencil_back.value_mask);
    glStencilMaskSeparate(GL_BACK, _glstate_backup.stencil_back.writemask);
    glStencilOpSeparate(GL_BACK, _glstate_backup.stencil_back.fail, _glstate_backup.stencil_back.pass_depth_fail,
                        _glstate_backup.stencil_back.pass_depth_pass);

    CheckGLError("EndFrame");
}
