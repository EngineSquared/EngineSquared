#pragma once

#include <RmlUi/Core.h>
#include <glfw/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

#include "Logger.hpp"
#include "Mesh.hpp"
#include "Object.hpp"
#include "OpenGL.hpp"
#include "Window.hpp"

// Determines the anti-aliasing quality when creating layers. Enables better-looking visuals, especially when transforms are applied.
#ifndef RMLUI_NUM_MSAA_SAMPLES
	#define RMLUI_NUM_MSAA_SAMPLES 2
#endif

namespace ES::Plugin::UI::Utils {
class RenderInterface : public Rml::RenderInterface {
  public:
    explicit RenderInterface(ES::Engine::Core &core) : _core(core) {}

  private:
    ES::Engine::Core &_core;

    struct GeometryRecord {
        Rml::CompiledGeometryHandle id;
        entt::hashed_string mesh_handle;
        ES::Plugin::Object::Component::Mesh mesh;
    };

    struct TextureRecord {
        Rml::TextureHandle id;
        entt::hashed_string texture_handle;
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

    /*
	 *   Manages render targets, including the layer stack and postprocessing framebuffers.
     *
	 *   Layers can be pushed and popped, creating new framebuffers as needed. Typically, geometry is rendered to the top
	 *   layer. The layer framebuffers may have MSAA enabled.
     *
	 *   Postprocessing framebuffers are separate from the layers, and are commonly used to apply texture-wide effects
	 *   such as filters. They are used both as input and output during rendering, and do not use MSAA.
	*/
    class RenderLayerStack {
	public:
		RenderLayerStack()
        {
            fb_postprocess.resize(4);
        }

		~RenderLayerStack()
        {
            DestroyFramebuffers();
        }

		// Push a new layer. All references to previously retrieved layers are invalidated.
		Rml::LayerHandle PushLayer()
        {
            RMLUI_ASSERT(layers_size <= (int)fb_layers.size());

            if (layers_size == (int)fb_layers.size())
            {
                // All framebuffers should share a single stencil buffer.
                GLuint shared_depth_stencil = (fb_layers.empty() ? 0 : fb_layers.front().depth_stencil_buffer);

                fb_layers.push_back(FramebufferData{});
                CreateFramebuffer(fb_layers.back(), width, height, RMLUI_NUM_MSAA_SAMPLES, FramebufferAttachment::DepthStencil,
                    shared_depth_stencil);
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


		const FramebufferData& GetLayer(Rml::LayerHandle layer) const
        {
            RMLUI_ASSERT((size_t)layer < (size_t)layers_size);
            return fb_layers[layer];
        }

		const FramebufferData& GetTopLayer() const
        {
            return GetLayer(GetTopLayerHandle());
        }

		Rml::LayerHandle GetTopLayerHandle() const
        {
            RMLUI_ASSERT(layers_size > 0);
            return static_cast<Rml::LayerHandle>(layers_size - 1);
        }

		const FramebufferData& GetPostprocessPrimary() { return EnsureFramebufferPostprocess(0); }
		const FramebufferData& GetPostprocessSecondary() { return EnsureFramebufferPostprocess(1); }
		const FramebufferData& GetPostprocessTertiary() { return EnsureFramebufferPostprocess(2); }
		const FramebufferData& GetBlendMask() { return EnsureFramebufferPostprocess(3); }

		void SwapPostprocessPrimarySecondary()
        {
            std::swap(fb_postprocess[0], fb_postprocess[1]);
        }

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
            RMLUI_ASSERTMSG(layers_size == 0, "Do not call this during frame rendering, that is, between BeginFrame() and EndFrame().");

            for (FramebufferData& fb : fb_layers)
                DestroyFramebuffer(fb);

            fb_layers.clear();

            for (FramebufferData& fb : fb_postprocess)
                DestroyFramebuffer(fb);
        }
		const FramebufferData& EnsureFramebufferPostprocess(int index)
        {
            RMLUI_ASSERT(index < (int)fb_postprocess.size())
            FramebufferData& fb = fb_postprocess[index];
            if (!fb.framebuffer)
                CreateFramebuffer(fb, width, height, 0, FramebufferAttachment::None, 0);
            return fb;
        }

		int width = 0, height = 0;

		// The number of active layers is manually tracked since we re-use the framebuffers stored in the fb_layers stack.
		int layers_size = 0;

		Rml::Vector<FramebufferData> fb_layers;
		Rml::Vector<FramebufferData> fb_postprocess;
	};

    GeometryRecord _geometry;
    TextureRecord _texture;

    GLStateBackup _glstate_backup = {};
    RenderLayerStack _render_layers;
    Rml::Rectanglei _scissor_state;
    // Rml::CompiledGeometryHandle fullscreen_quad_geometry = {};
    enum class FramebufferAttachment { None, DepthStencil };

    static constexpr Rml::TextureHandle TexturePostprocess = Rml::TextureHandle(-2);

    static bool CreateFramebuffer(FramebufferData& out_fb, int width, int height, int samples, FramebufferAttachment attachment,
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
            Rml::Log::Message(Rml::Log::LT_ERROR, "OpenGL framebuffer could not be generated. Error code %x.", framebuffer_status);
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

    static void DestroyFramebuffer(FramebufferData& fb)
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
        auto &mesh = _geometry.mesh;

        mesh.vertices.reserve(vertices.size());
        mesh.normals.resize(vertices.size(), glm::vec3(0.0f));
        mesh.texCoords.reserve(vertices.size());

        for (const auto &v : vertices) {
            std::cout << v.tex_coord.x << " " << v.tex_coord.y << std::endl;
            mesh.vertices.emplace_back(v.position.x, v.position.y, 0.0f);
            mesh.texCoords.emplace_back(v.tex_coord.x, v.tex_coord.y);
        }
        mesh.indices.assign(indices.begin(), indices.end());

        _geometry.mesh_handle = entt::hashed_string{"rml_mesh"};

        auto &bufferManager = _core.GetResource<ES::Plugin::OpenGL::Resource::GLMeshBufferManager>();
        if (!bufferManager.Contains(_geometry.mesh_handle)) {
            ES::Plugin::OpenGL::Utils::GLMeshBuffer buffer;
            buffer.GenerateGLMeshBuffers(mesh);
            bufferManager.Add(_geometry.mesh_handle, buffer);
        }

        _geometry.id = 1; // TO CHANGE ??
        return _geometry.id;
    }

    void RenderGeometry(Rml::CompiledGeometryHandle handle, Rml::Vector2f translation,
                        Rml::TextureHandle texture_handle) override
    {
        auto &mesh = _geometry.mesh;
        auto &mesh_handle = _geometry.mesh_handle;
        auto &shaderManager = _core.GetResource<ES::Plugin::OpenGL::Resource::ShaderManager>();
        auto &texShaderProg = shaderManager.Get("RmlVertexTexture");
        auto &vertColShaderProg = shaderManager.Get("RmlVertexColor");
        bool hasTexture = false;

        if (texture_handle != TexturePostprocess)
        {
            auto &textureManager = _core.GetResource<ES::Plugin::OpenGL::Resource::TextureManager>();
            auto &tex = textureManager.Get(_texture.texture_handle);
            if (tex.IsValid()) {
                hasTexture = true;
                texShaderProg.Use();
                tex.Bind();
                glUniform1i(texShaderProg.GetUniform("_tex"), 0);
            } else {
                vertColShaderProg.Use();
                glBindTexture(GL_TEXTURE_2D, 0);
            }
        }

        auto &bufferManager = _core.GetResource<ES::Plugin::OpenGL::Resource::GLMeshBufferManager>();
        auto &buffer = bufferManager.Get(mesh_handle);

        auto &size = _core.GetResource<ES::Plugin::OpenGL::Resource::Camera>().size;
        glm::mat4 projection = glm::ortho(0.0f, size.x, 0.0f, size.y, -1.0f, 1.0f);
        if (hasTexture)
        {
            glUniform2f(texShaderProg.GetUniform("_translate"), translation.x, translation.y);
            glUniformMatrix4fv(texShaderProg.GetUniform("_transform"), 1, GL_FALSE, glm::value_ptr(projection));
            buffer.Draw(mesh);
            texShaderProg.Disable();
        }
        else
        {
            glUniform2f(vertColShaderProg.GetUniform("_translate"), translation.x, translation.y);
            glUniformMatrix4fv(vertColShaderProg.GetUniform("_transform"), 1, GL_FALSE, glm::value_ptr(projection));
            buffer.Draw(mesh);
            vertColShaderProg.Disable();
        }
    }

    void ReleaseGeometry(Rml::CompiledGeometryHandle handle) override
    {
        const auto &mesh_handle = _geometry.mesh_handle;
        auto &bufferManager = _core.GetResource<ES::Plugin::OpenGL::Resource::GLMeshBufferManager>();
        bufferManager.Remove(_geometry.mesh_handle);
    }

    Rml::TextureHandle LoadTexture(Rml::Vector2i &texture_dimensions, const Rml::String &source) override
    {
        _texture.texture_handle = entt::hashed_string{"rml_texture"};

        auto &textureManager = _core.GetResource<ES::Plugin::OpenGL::Resource::TextureManager>();
        if (!textureManager.Contains(_texture.texture_handle)) {
            ES::Plugin::OpenGL::Utils::Texture tex(source.data());
            textureManager.Add(_texture.texture_handle, tex);
        }

        auto &texture = textureManager.Get(_texture.texture_handle);
        if (!texture.IsValid()) {
            ES::Utils::Log::Error(fmt::format("RmlUi: Loaded texture {} is not valid", _texture.texture_handle.data()));
            return 0;
        }

        texture_dimensions = {texture.GetWidth(), texture.GetHeight()};

        _texture.id = 1;
        return _texture.id;
    }

    Rml::TextureHandle GenerateTexture(Rml::Span<const Rml::byte> source, Rml::Vector2i dimensions) override
    {
        std::cout << "Generate Texture called" << std::endl;
        // Called if text is used
        return 0;
    }

    void ReleaseTexture(Rml::TextureHandle handle) override
    {
        auto &textureManager = _core.GetResource<ES::Plugin::OpenGL::Resource::TextureManager>();
        textureManager.Remove(_texture.texture_handle);
    }

    void EnableScissorRegion(bool enable) override { enable ? glEnable(GL_SCISSOR_TEST) : glDisable(GL_SCISSOR_TEST); }

    void SetScissorRegion(Rml::Rectanglei region) override
    {
        glScissor(region.Left(), region.Top(), region.Width(), region.Height());
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
        // We do blending in nonlinear sRGB space because that is the common practice and gives results that we are used to.
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

        auto &shaderManager = _core.GetResource<ES::Plugin::OpenGL::Resource::ShaderManager>();
        auto &sp = shaderManager.Get("RmlVertexTexture");
        sp.Disable();
        // UseProgram(ProgramId::None);
        // program_transform_dirty.set();
        _scissor_state = Rml::Rectanglei::MakeInvalid();

        // Gfx::CheckGLError("BeginFrame");
    }

    void EndFrame()
    {
        const FramebufferData& fb_active = _render_layers.GetTopLayer();
        const FramebufferData& fb_postprocess = _render_layers.GetPostprocessPrimary();
        const auto &windowSize = _core.GetResource<ES::Plugin::Window::Resource::Window>().GetSize();

        // Resolve MSAA to postprocess framebuffer.
        glBindFramebuffer(GL_READ_FRAMEBUFFER, fb_active.framebuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb_postprocess.framebuffer);

        glBlitFramebuffer(0, 0, fb_active.width, fb_active.height, 0, 0, fb_postprocess.width, fb_postprocess.height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        // Draw to backbuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // glViewport(viewport_offset_x, viewport_offset_y, windowSize.x, windowSize.y);

        // Assuming we have an opaque background, we can just write to it with the premultiplied alpha blend mode and we'll get the correct result.
        // Instead, if we had a transparent destination that didn't use premultiplied alpha, we would need to perform a manual un-premultiplication step.
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, fb_postprocess.color_tex_buffer);
        // UseProgram(ProgramId::Passthrough);
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

        glViewport(_glstate_backup.viewport[0], _glstate_backup.viewport[1], _glstate_backup.viewport[2], _glstate_backup.viewport[3]);
        glScissor(_glstate_backup.scissor[0], _glstate_backup.scissor[1], _glstate_backup.scissor[2], _glstate_backup.scissor[3]);

        glActiveTexture(_glstate_backup.active_texture);

        glClearStencil(_glstate_backup.stencil_clear_value);
        glClearColor(_glstate_backup.color_clear_value[0], _glstate_backup.color_clear_value[1], _glstate_backup.color_clear_value[2],
            _glstate_backup.color_clear_value[3]);
        glColorMask(_glstate_backup.color_writemask[0], _glstate_backup.color_writemask[1], _glstate_backup.color_writemask[2],
            _glstate_backup.color_writemask[3]);

        glBlendEquationSeparate(_glstate_backup.blend_equation_rgb, _glstate_backup.blend_equation_alpha);
        glBlendFuncSeparate(_glstate_backup.blend_src_rgb, _glstate_backup.blend_dst_rgb, _glstate_backup.blend_src_alpha, _glstate_backup.blend_dst_alpha);

        glStencilFuncSeparate(GL_FRONT, _glstate_backup.stencil_front.func, _glstate_backup.stencil_front.ref, _glstate_backup.stencil_front.value_mask);
        glStencilMaskSeparate(GL_FRONT, _glstate_backup.stencil_front.writemask);
        glStencilOpSeparate(GL_FRONT, _glstate_backup.stencil_front.fail, _glstate_backup.stencil_front.pass_depth_fail,
            _glstate_backup.stencil_front.pass_depth_pass);

        glStencilFuncSeparate(GL_BACK, _glstate_backup.stencil_back.func, _glstate_backup.stencil_back.ref, _glstate_backup.stencil_back.value_mask);
        glStencilMaskSeparate(GL_BACK, _glstate_backup.stencil_back.writemask);
        glStencilOpSeparate(GL_BACK, _glstate_backup.stencil_back.fail, _glstate_backup.stencil_back.pass_depth_fail,
            _glstate_backup.stencil_back.pass_depth_pass);

        // CheckGLError("EndFrame");
    }

    void DrawFullscreenQuad()
    {
        RenderGeometry(_geometry.id, {}, TexturePostprocess);
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
