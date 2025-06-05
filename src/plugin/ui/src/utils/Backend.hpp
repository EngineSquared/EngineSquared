#pragma once

#include <RmlUi/Core.h>
#include <glfw/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

#include "Logger.hpp"
#include "Mesh.hpp"
#include "Object.hpp"
#include "OpenGL.hpp"
#include "Window.hpp"

// Determines the anti-aliasing quality when creating layers. Enables better-looking visuals, especially when transforms
// are applied.
#ifndef RMLUI_NUM_MSAA_SAMPLES
#    define RMLUI_NUM_MSAA_SAMPLES 2
#endif

namespace ES::Plugin::UI::Utils {
class RenderInterface : public Rml::RenderInterface {
  public:
    explicit RenderInterface(ES::Engine::Core &core) : _core(core){};
    ~RenderInterface() = default;

  private:
    ES::Engine::Core &_core;

    // struct GeometryRecord {
    //     entt::hashed_string mesh_handle;
    //     ES::Plugin::Object::Component::Mesh mesh;
    // };

    struct CompiledGeometryData {
        entt::hashed_string mesh_handle;
        GLuint vao;
        GLuint vbo;
        GLuint ibo;
        GLsizei draw_count;
    };

    struct GLStateBackup {
        bool enable_cull_face;
        bool enable_blend;
        bool enable_stencil_test;
        bool enable_scissor_test;
        bool enable_depth_test;

        int viewport[4];
        int scissor[4];

        int active_texture;

        int stencil_clear_value;
        float color_clear_value[4];
        unsigned char color_writemask[4];

        int blend_equation_rgb;
        int blend_equation_alpha;
        int blend_src_rgb;
        int blend_dst_rgb;
        int blend_src_alpha;
        int blend_dst_alpha;

        struct Stencil {
            int func;
            int ref;
            int value_mask;
            int writemask;
            int fail;
            int pass_depth_fail;
            int pass_depth_pass;
        };
        Stencil stencil_front;
        Stencil stencil_back;
    };

    struct FramebufferData {
        int width, height;
        GLuint framebuffer;
        GLuint color_tex_buffer;
        GLuint color_render_buffer;
        GLuint depth_stencil_buffer;
        bool owns_depth_stencil_buffer;
    };

// Set to byte packing, or the compiler will expand our struct, which means it won't read correctly from file
#pragma pack(1)
    struct TGAHeader {
        char idLength;
        char colourMapType;
        char dataType;
        short int colourMapOrigin;
        short int colourMapLength;
        char colourMapDepth;
        short int xOrigin;
        short int yOrigin;
        short int width;
        short int height;
        char bitsPerPixel;
        char imageDescriptor;
    };
// Restore packing
#pragma pack()

    /*
     *   Manages render targets, including the layer stack and postprocessing framebuffers.
     *
     *   Layers can be pushed and popped, creating new framebuffers as needed. Typically, geometry is rendered to the
     * top layer. The layer framebuffers may have MSAA enabled.
     *
     *   Postprocessing framebuffers are separate from the layers, and are commonly used to apply texture-wide effects
     *   such as filters. They are used both as input and output during rendering, and do not use MSAA.
     */
    class RenderLayerStack {
      public:
        RenderLayerStack() { fb_postprocess.resize(4); }

        ~RenderLayerStack() { DestroyFramebuffers(); }

        // Push a new layer. All references to previously retrieved layers are invalidated.
        Rml::LayerHandle PushLayer()
        {
            RMLUI_ASSERT(layers_size <= static_cast<int>(fb_layers.size()));

            if (layers_size == static_cast<int>(fb_layers.size()))
            {
                // All framebuffers should share a single stencil buffer.
                GLuint shared_depth_stencil = (fb_layers.empty() ? 0 : fb_layers.front().depth_stencil_buffer);

                fb_layers.push_back(FramebufferData{});
                CreateFramebuffer(fb_layers.back(), width, height, RMLUI_NUM_MSAA_SAMPLES,
                                  FramebufferAttachment::DepthStencil, shared_depth_stencil);
            }

            layers_size += 1;
            return GetTopLayerHandle();
        }

        // Pop the top layer. All references to previously retrieved layers are invalidated.
        void PopLayer()
        {
            RMLUI_ASSERT(layers_size > 0);
            layers_size -= 1;
        }

        const FramebufferData &GetLayer(Rml::LayerHandle layer) const
        {
            RMLUI_ASSERT((size_t) layer < (size_t) layers_size);
            return fb_layers[layer];
        }

        const FramebufferData &GetTopLayer() const { return GetLayer(GetTopLayerHandle()); }

        Rml::LayerHandle GetTopLayerHandle() const
        {
            RMLUI_ASSERT(layers_size > 0);
            return static_cast<Rml::LayerHandle>(layers_size - 1);
        }

        const FramebufferData &GetPostprocessPrimary() { return EnsureFramebufferPostprocess(0); }
        const FramebufferData &GetPostprocessSecondary() { return EnsureFramebufferPostprocess(1); }
        const FramebufferData &GetPostprocessTertiary() { return EnsureFramebufferPostprocess(2); }
        const FramebufferData &GetBlendMask() { return EnsureFramebufferPostprocess(3); }

        void SwapPostprocessPrimarySecondary() { std::swap(fb_postprocess[0], fb_postprocess[1]); }

        void BeginFrame(int new_width, int new_height)
        {
            RMLUI_ASSERT(layers_size == 0);

            if (new_width != width || new_height != height)
            {
                width = new_width;
                height = new_height;

                DestroyFramebuffers();
            }

            PushLayer();
        }

        void EndFrame()
        {
            RMLUI_ASSERT(layers_size == 1);
            PopLayer();
        }

      private:
        void DestroyFramebuffers()
        {
            RMLUI_ASSERTMSG(layers_size == 0,
                            "Do not call this during frame rendering, that is, between BeginFrame() and EndFrame().");

            for (FramebufferData &fb : fb_layers)
                DestroyFramebuffer(fb);

            fb_layers.clear();

            for (FramebufferData &fb : fb_postprocess)
                DestroyFramebuffer(fb);
        }
        const FramebufferData &EnsureFramebufferPostprocess(int index)
        {
            RMLUI_ASSERT(index < static_cast<int>(fb_postprocess.size()))
            FramebufferData &fb = fb_postprocess[index];
            if (!fb.framebuffer)
                CreateFramebuffer(fb, width, height, 0, FramebufferAttachment::None, 0);
            return fb;
        }

        int width = 0, height = 0;

        // The number of active layers is manually tracked since we re-use the framebuffers stored in the fb_layers
        // stack.
        int layers_size = 0;

        Rml::Vector<FramebufferData> fb_layers;
        Rml::Vector<FramebufferData> fb_postprocess;
    };

    std::unordered_map<Rml::CompiledGeometryHandle, CompiledGeometryData> _geometries;
    std::unordered_map<Rml::TextureHandle, std::string> _textures;

    Rml::CompiledGeometryHandle _next_geom_id = 1u;
    Rml::TextureHandle _next_tex_id = 1u;

    GLStateBackup _glstate_backup = {};
    RenderLayerStack _render_layers;
    Rml::Rectanglei _scissor_state;
    entt::hashed_string activeShaderProgram = "";
    enum class FramebufferAttachment {
        None,
        DepthStencil
    };

    enum class VertexAttribute {
        Position,
        Color0,
        TexCoord0,
        Count
    };

    static constexpr Rml::TextureHandle TextureEnableWithoutBinding = Rml::TextureHandle(-1);
    static constexpr Rml::TextureHandle TexturePostprocess = Rml::TextureHandle(-2);

    void UseShaderProgram(const entt::hashed_string &program_id)
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

    void DisableActiveShaderProgram()
    {
        auto &shaderManager = _core.GetResource<ES::Plugin::OpenGL::Resource::ShaderManager>();
        shaderManager.Get(activeShaderProgram).Use();
    }

    /// Flip the vertical axis of the rectangle, and move its origin to the vertically opposite side of the viewport.
    /// @note Changes the coordinate system from RmlUi to OpenGL, or equivalently in reverse.
    /// @note The Rectangle::Top and Rectangle::Bottom members will have reverse meaning in the returned rectangle.
    static Rml::Rectanglei VerticallyFlipped(Rml::Rectanglei rect, int viewport_height)
    {
        RMLUI_ASSERT(rect.Valid());
        Rml::Rectanglei flipped_rect = rect;
        flipped_rect.p0.y = viewport_height - rect.p1.y;
        flipped_rect.p1.y = viewport_height - rect.p0.y;
        return flipped_rect;
    }

    static bool CreateFramebuffer(FramebufferData &out_fb, int width, int height, int samples,
                                  FramebufferAttachment attachment, GLuint shared_depth_stencil_buffer)
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

            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER,
                                      depth_stencil_buffer);
        }

        const GLuint framebuffer_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (framebuffer_status != GL_FRAMEBUFFER_COMPLETE)
        {
            Rml::Log::Message(Rml::Log::LT_ERROR, "OpenGL framebuffer could not be generated. Error code %x.",
                              framebuffer_status);
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

        return true;
    }

    static void DestroyFramebuffer(FramebufferData &fb)
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
    }

  public:
    Rml::CompiledGeometryHandle CompileGeometry(Rml::Span<const Rml::Vertex> vertices,
                                                Rml::Span<const int> indices) override
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
        glVertexAttribPointer(static_cast<GLuint>(VertexAttribute::Position), 2, GL_FLOAT, GL_FALSE,
                              sizeof(Rml::Vertex), reinterpret_cast<const GLvoid *>(offsetof(Rml::Vertex, position)));

        glEnableVertexAttribArray(static_cast<GLuint>(VertexAttribute::Color0));
        glVertexAttribPointer(static_cast<GLuint>(VertexAttribute::Color0), 4, GL_UNSIGNED_BYTE, GL_TRUE,
                              sizeof(Rml::Vertex), reinterpret_cast<const GLvoid *>(offsetof(Rml::Vertex, colour)));

        glEnableVertexAttribArray(static_cast<GLuint>(VertexAttribute::TexCoord0));
        glVertexAttribPointer(static_cast<GLuint>(VertexAttribute::TexCoord0), 2, GL_FLOAT, GL_FALSE,
                              sizeof(Rml::Vertex), reinterpret_cast<const GLvoid *>(offsetof(Rml::Vertex, tex_coord)));

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indices.size(),
                     reinterpret_cast<const void *>(indices.data()), draw_usage);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        std::string key = "rml_mesh_" + std::to_string(_next_geom_id);
        CompiledGeometryData geometry;
        geometry.mesh_handle = entt::hashed_string{key.c_str()};
        geometry.vao = vao;
        geometry.vbo = vbo;
        geometry.ibo = ibo;
        geometry.draw_count = (GLsizei) indices.size();
        Rml::CompiledGeometryHandle id = _next_geom_id++;

        _geometries.emplace(id, std::move(geometry));
        ES::Utils::Log::Info(fmt::format("Rmlui: Compiled geometry for {}", geometry.mesh_handle.data()));
        return id;
    }

    void RenderGeometry(Rml::CompiledGeometryHandle handle, Rml::Vector2f translation,
                        Rml::TextureHandle texture_handle) override
    {
        const auto it = _geometries.find(handle);
        if (it == _geometries.end())
            return;

        auto &shaderManager = _core.GetResource<ES::Plugin::OpenGL::Resource::ShaderManager>();
        auto &size = _core.GetResource<ES::Plugin::OpenGL::Resource::Camera>().size;
        glm::mat4 projection = glm::ortho(0.0f, size.x, 0.0f, size.y, -1.0f, 1.0f);

        if (texture_handle == TexturePostprocess)
        {
            // Do nothing.
        }
        else if (texture_handle)
        {
            UseShaderProgram("RmlVertexTexture");
            // SubmitTransformUniform(translation);
            if (texture_handle != TextureEnableWithoutBinding)
            {
                const auto &texIt = _textures.find(texture_handle);
                if (texIt != _textures.end())
                {
                    auto &tex = _core.GetResource<ES::Plugin::OpenGL::Resource::TextureManager>().Get(
                        entt::hashed_string{texIt->second.c_str()});
                    glBindTexture(GL_TEXTURE_2D, tex.GetTexID());
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
            glUniform2f(vertColShaderProg.GetUniform("_translate"), translation.x, translation.y);
            glUniformMatrix4fv(vertColShaderProg.GetUniform("_transform"), 1, GL_FALSE, glm::value_ptr(projection));
            // SubmitTransformUniform(translation);
        }

        glBindVertexArray(it->second.vao);
        glDrawElements(GL_TRIANGLES, it->second.draw_count, GL_UNSIGNED_INT, reinterpret_cast<const GLvoid *>(0));

        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void ReleaseGeometry(Rml::CompiledGeometryHandle handle) override
    {
        auto it = _geometries.find(handle);
        if (it != _geometries.end())
        {
            auto &bufferManager = _core.GetResource<ES::Plugin::OpenGL::Resource::GLMeshBufferManager>();
            if (bufferManager.Contains(it->second.mesh_handle))
            {
                glDeleteVertexArrays(1, &it->second.vao);
                glDeleteBuffers(1, &it->second.vbo);
                glDeleteBuffers(1, &it->second.ibo);
                bufferManager.Remove(it->second.mesh_handle);
            }
            _geometries.erase(it);
        }
    }

    Rml::TextureHandle LoadTexture(Rml::Vector2i &texture_dimensions, const Rml::String &source) override
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

        Rml::TextureHandle id = _next_tex_id;
        _textures[id] = key;
        _next_tex_id += 1;
        return id;
    }

    Rml::TextureHandle GenerateTexture(Rml::Span<const Rml::byte> source, Rml::Vector2i dimensions) override
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

        _textures[texture_id] = key;
        _next_tex_id += 1;
        return texture_id;
    }

    Rml::TextureHandle CreateTexture(Rml::Span<const Rml::byte> source_data, Rml::Vector2i source_dimensions)
    {
        GLuint texture_id = static_cast<GLuint>(_next_tex_id);
        glGenTextures(1, &texture_id);
        if (texture_id == 0)
            return 0;

        glBindTexture(GL_TEXTURE_2D, texture_id);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, source_dimensions.x, source_dimensions.y, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                     source_data.data());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glBindTexture(GL_TEXTURE_2D, 0);

        return static_cast<Rml::TextureHandle>(texture_id);
    }

    void ReleaseTexture(Rml::TextureHandle handle) override
    {
        auto it = _textures.find(handle);
        if (it != _textures.end())
        {
            auto &textureManager = _core.GetResource<ES::Plugin::OpenGL::Resource::TextureManager>();
            textureManager.Remove(entt::hashed_string{it->second.c_str()});
            _textures.erase(it);
        }
    }

    void EnableScissorRegion(bool enable) override
    {
        if (enable)
            SetScissor(Rml::Rectanglei::MakeInvalid(), false);
    }

    void SetScissorRegion(Rml::Rectanglei region) override { SetScissor(region, true); }

    void SetScissor(Rml::Rectanglei region, bool vertically_flip)
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
    }

    Rml::CompiledShaderHandle CompileShader(const Rml::String & /*name*/, const Rml::Dictionary & /*parameters*/)
    {
        std::cout << "compile shader" << std::endl;
        return 0;
    }

    void RenderShader(Rml::CompiledShaderHandle /*shader_handle*/, Rml::CompiledGeometryHandle /*geometry_handle*/,
                      Rml::Vector2f /*translation*/, Rml::TextureHandle /*texture*/)
    {
        std::cout << "render shader" << std::endl;
    }

    void BeginFrame()
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
        // program_transform_dirty.set();
        _scissor_state = Rml::Rectanglei::MakeInvalid();

        // Gfx::CheckGLError("BeginFrame");
    }

    void EndFrame(ES::Engine::Core &)
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
        // UseShaderProgram("RmlPassthrough");
        DrawFullscreenQuad();

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
        glBlendFuncSeparate(_glstate_backup.blend_src_rgb, _glstate_backup.blend_dst_rgb,
                            _glstate_backup.blend_src_alpha, _glstate_backup.blend_dst_alpha);

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

        // CheckGLError("EndFrame");
    }

    void DrawFullscreenQuad()
    {
        for (auto &[id, hash] : _textures)
        {
            RenderGeometry(id, {}, TexturePostprocess);
        }
    }
};

class SystemInterface : public Rml::SystemInterface {
    double GetElapsedTime() override { return glfwGetTime(); }

    bool LogMessage(Rml::Log::Type type, const Rml::String &message) override
    {
        switch (type)
        {
        case Rml::Log::Type::LT_ASSERT:
        case Rml::Log::Type::LT_ERROR: ES::Utils::Log::Error(fmt::format("RmlUi: {}", message)); break;
        case Rml::Log::Type::LT_WARNING: ES::Utils::Log::Warn(fmt::format("RmlUi: {}", message)); break;
        case Rml::Log::Type::LT_INFO: ES::Utils::Log::Info(fmt::format("RmlUi: {}", message)); break;
        default: break;
        }
        return true;
    }
};

} // namespace ES::Plugin::UI::Utils
