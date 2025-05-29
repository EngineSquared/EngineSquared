#pragma once

#include <RmlUi/Core.h>
#include <glfw/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

#include "Logger.hpp"
#include "Mesh.hpp"
#include "Object.hpp"
#include "OpenGL.hpp"
#include "Window.hpp"

namespace ES::Plugin::UI::Utils {
class RenderInterface : public Rml::RenderInterface {
  public:
    explicit RenderInterface(ES::Engine::Core &core) : _core(core) {}

  private:
    ES::Engine::Core &_core;

    struct GeometryRecord {
        entt::hashed_string mesh_handle;
        ES::Plugin::Object::Component::Mesh mesh;
    };

    // struct GLStateBackup {
    // 	bool enable_cull_face;
    // 	bool enable_blend;
    // 	bool enable_stencil_test;
    // 	bool enable_scissor_test;
    // 	bool enable_depth_test;

    // 	int viewport[4];
    // 	int scissor[4];

    // 	int active_texture;

    // 	int stencil_clear_value;
    // 	float color_clear_value[4];
    // 	unsigned char color_writemask[4];

    // 	int blend_equation_rgb;
    // 	int blend_equation_alpha;
    // 	int blend_src_rgb;
    // 	int blend_dst_rgb;
    // 	int blend_src_alpha;
    // 	int blend_dst_alpha;

    // 	struct Stencil {
    // 		int func;
    // 		int ref;
    // 		int value_mask;
    // 		int writemask;
    // 		int fail;
    // 		int pass_depth_fail;
    // 		int pass_depth_pass;
    // 	};
    // 	Stencil stencil_front;
    // 	Stencil stencil_back;
    // };

    std::unordered_map<Rml::CompiledGeometryHandle, GeometryRecord> _geometry_map;
    std::unordered_map<Rml::TextureHandle, entt::hashed_string> _texture_handle_map;

    uintptr_t _next_geometry_id = 1;
    uintptr_t _next_texture_id = 1;

    // GLStateBackup _glstate_backup = {};
    // RenderLayerStack render_layers;

    /// Flip the vertical axis of the rectangle, and move its origin to the vertically opposite side of the viewport.
    /// @note Changes the coordinate system from RmlUi to OpenGL, or equivalently in reverse.
    /// @note The Rectangle::Top and Rectangle::Bottom members will have reverse meaning in the returned rectangle.
    Rml::Rectanglei VerticallyFlipped(Rml::Rectanglei rect, int viewport_height)
    {
        RMLUI_ASSERT(rect.Valid());
        Rml::Rectanglei flipped_rect = rect;
        flipped_rect.p0.y = viewport_height - rect.p1.y;
        flipped_rect.p1.y = viewport_height - rect.p0.y;
        return flipped_rect;
    }

  public:
    Rml::CompiledGeometryHandle CompileGeometry(Rml::Span<const Rml::Vertex> vertices,
                                                Rml::Span<const int> indices) override
    {
        GeometryRecord record;
        auto &mesh = record.mesh;

        mesh.vertices.reserve(vertices.size());
        mesh.normals.resize(vertices.size(), glm::vec3(0.0f));
        mesh.texCoords.reserve(vertices.size());

        for (const auto &v : vertices)
        {
            mesh.vertices.emplace_back(v.position.x, v.position.y, 0.0f);
            mesh.texCoords.emplace_back(v.tex_coord.x, v.tex_coord.y);
        }
        mesh.indices.assign(indices.begin(), indices.end());

        auto handle_id = fmt::format("rml_mesh_{}", _next_geometry_id);
        entt::hashed_string mesh_handle = entt::hashed_string(handle_id.c_str());

        auto &bufferManager = _core.GetResource<ES::Plugin::OpenGL::Resource::GLMeshBufferManager>();
        if (!bufferManager.Contains(mesh_handle))
        {
            ES::Plugin::OpenGL::Utils::GLMeshBuffer buffer;
            buffer.GenerateGLMeshBuffers(mesh);
            bufferManager.Add(mesh_handle, buffer);
        }

        record.mesh_handle = mesh_handle;
        Rml::CompiledGeometryHandle id = _next_geometry_id++;
        _geometry_map.emplace(id, std::move(record));
        return id;
    }

    void RenderGeometry(Rml::CompiledGeometryHandle handle, Rml::Vector2f translation,
                        Rml::TextureHandle texture_handle) override
    {
        const auto it = _geometry_map.find(handle);
        if (it == _geometry_map.end())
            return;

        auto &mesh = it->second.mesh;
        auto &mesh_handle = it->second.mesh_handle;
        auto &shaderManager = _core.GetResource<ES::Plugin::OpenGL::Resource::ShaderManager>();
        entt::hashed_string shaderProgram = "RmlVertexColor";

        auto tex_it = _texture_handle_map.find(texture_handle);
        if (tex_it != _texture_handle_map.end())
        {
            auto &textureManager = _core.GetResource<ES::Plugin::OpenGL::Resource::TextureManager>();
            auto &tex = textureManager.Get(tex_it->second);
            if (tex.IsValid())
            {
                tex.Bind();
                shaderProgram = "RmlVertexTexture";
                auto &sp = shaderManager.Get(shaderProgram);
                sp.Use();
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, tex.GetTexID());
                glUniform1i(sp.GetUniform("_tex"), 0);
            }
            else
            {
                glBindTexture(GL_TEXTURE_2D, 0);
                auto &sp = shaderManager.Get(shaderProgram);
                sp.Use();
            }
        }
        else
        {
            glBindTexture(GL_TEXTURE_2D, 0);
            auto &sp = shaderManager.Get(shaderProgram);
            sp.Use();
        }

        auto &bufferManager = _core.GetResource<ES::Plugin::OpenGL::Resource::GLMeshBufferManager>();
        auto &buffer = bufferManager.Get(mesh_handle);

        auto &sp = shaderManager.Get(shaderProgram);
        glUniform2f(sp.GetUniform("_translate"), translation.x, translation.y);
        // glUniformMatrix4fv(sp.GetUniform("_transform"), 1, GL_FALSE, glm::value_ptr(transform_matrix));
        glUniform2f(sp.GetUniform("_transform"), translation.x, translation.y);
        buffer.Draw(mesh);
        sp.Disable();
    }

    void ReleaseGeometry(Rml::CompiledGeometryHandle handle) override
    {
        auto it = _geometry_map.find(handle);
        if (it != _geometry_map.end())
        {
            const auto &mesh_handle = it->second.mesh_handle;
            auto &bufferManager = _core.GetResource<ES::Plugin::OpenGL::Resource::GLMeshBufferManager>();
            if (bufferManager.Contains(it->second.mesh_handle))
            {
                bufferManager.Remove(it->second.mesh_handle);
            }
            _geometry_map.erase(it);
        }
    }

    Rml::TextureHandle LoadTexture(Rml::Vector2i &texture_dimensions, const Rml::String &source) override
    {
        auto texture_id = fmt::format("rml_texture_{}", _next_texture_id);
        entt::hashed_string handle = entt::hashed_string(texture_id.c_str());

        auto &textureManager = _core.GetResource<ES::Plugin::OpenGL::Resource::TextureManager>();
        if (!textureManager.Contains(handle))
        {
            ES::Plugin::OpenGL::Utils::Texture tex(source.c_str());
            textureManager.Add(handle, tex);
        }

        auto &texture = textureManager.Get(handle);
        if (!texture.IsValid())
        {
            ES::Utils::Log::Error(fmt::format("RmlUi: Loaded texture {} is not valid", texture_id));
            return 0;
        }

        texture_dimensions = {texture.GetWidth(), texture.GetHeight()};

        Rml::TextureHandle id = _next_texture_id++;
        _texture_handle_map[id] = handle;
        return id;
    }

    Rml::TextureHandle GenerateTexture(Rml::Span<const Rml::byte> source, Rml::Vector2i dimensions) override
    {
        auto texture_id = fmt::format("rml_dynamic_texture_{}", _next_texture_id);
        entt::hashed_string handle = entt::hashed_string(texture_id.c_str());

        auto &textureManager = _core.GetResource<ES::Plugin::OpenGL::Resource::TextureManager>();
        if (!textureManager.Contains(handle))
        {
            ES::Plugin::OpenGL::Utils::Texture tex(source.data(), dimensions.x, dimensions.y);
            textureManager.Add(handle, tex);
        }

        auto &texture = textureManager.Get(handle);
        if (!texture.IsValid())
        {
            ES::Utils::Log::Error(fmt::format("RmlUi: Generated texture {} is not valid", texture_id));
            return 0;
        }

        Rml::TextureHandle id = _next_texture_id++;
        _texture_handle_map[id] = handle;
        return id;
    }

    void ReleaseTexture(Rml::TextureHandle handle) override
    {
        auto it = _texture_handle_map.find(handle);
        if (it != _texture_handle_map.end())
        {
            auto &textureManager = _core.GetResource<ES::Plugin::OpenGL::Resource::TextureManager>();
            textureManager.Remove(it->second);
            _texture_handle_map.erase(it);
        }
    }

    void EnableScissorRegion(bool enable) override { enable ? glEnable(GL_SCISSOR_TEST) : glDisable(GL_SCISSOR_TEST); }

    void SetScissorRegion(Rml::Rectanglei region) override
    {
        glScissor(region.Left(), region.Top(), region.Width(), region.Height());
    }

    // Sets up OpenGL states for taking rendering commands from RmlUi.
    // void BeginFrame()
    // {
    //     const auto &windowSize = _core.GetResource<ES::Plugin::Window::Resource::Window>().GetSize();

    //     RMLUI_ASSERT(windowSize.x >= 1 && windowSize.y >= 1);

    //     // Backup GL state.
    //     _glstate_backup.enable_cull_face = glIsEnabled(GL_CULL_FACE);
    //     _glstate_backup.enable_blend = glIsEnabled(GL_BLEND);
    //     _glstate_backup.enable_stencil_test = glIsEnabled(GL_STENCIL_TEST);
    //     _glstate_backup.enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);
    //     _glstate_backup.enable_depth_test = glIsEnabled(GL_DEPTH_TEST);

    //     glGetIntegerv(GL_VIEWPORT, _glstate_backup.viewport);
    //     glGetIntegerv(GL_SCISSOR_BOX, _glstate_backup.scissor);

    //     glGetIntegerv(GL_ACTIVE_TEXTURE, &_glstate_backup.active_texture);

    //     glGetIntegerv(GL_STENCIL_CLEAR_VALUE, &_glstate_backup.stencil_clear_value);
    //     glGetFloatv(GL_COLOR_CLEAR_VALUE, _glstate_backup.color_clear_value);
    //     glGetBooleanv(GL_COLOR_WRITEMASK, _glstate_backup.color_writemask);

    //     glGetIntegerv(GL_BLEND_EQUATION_RGB, &_glstate_backup.blend_equation_rgb);
    //     glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &_glstate_backup.blend_equation_alpha);
    //     glGetIntegerv(GL_BLEND_SRC_RGB, &_glstate_backup.blend_src_rgb);
    //     glGetIntegerv(GL_BLEND_DST_RGB, &_glstate_backup.blend_dst_rgb);
    //     glGetIntegerv(GL_BLEND_SRC_ALPHA, &_glstate_backup.blend_src_alpha);
    //     glGetIntegerv(GL_BLEND_DST_ALPHA, &_glstate_backup.blend_dst_alpha);

    //     glGetIntegerv(GL_STENCIL_FUNC, &_glstate_backup.stencil_front.func);
    //     glGetIntegerv(GL_STENCIL_REF, &_glstate_backup.stencil_front.ref);
    //     glGetIntegerv(GL_STENCIL_VALUE_MASK, &_glstate_backup.stencil_front.value_mask);
    //     glGetIntegerv(GL_STENCIL_WRITEMASK, &_glstate_backup.stencil_front.writemask);
    //     glGetIntegerv(GL_STENCIL_FAIL, &_glstate_backup.stencil_front.fail);
    //     glGetIntegerv(GL_STENCIL_PASS_DEPTH_FAIL, &_glstate_backup.stencil_front.pass_depth_fail);
    //     glGetIntegerv(GL_STENCIL_PASS_DEPTH_PASS, &_glstate_backup.stencil_front.pass_depth_pass);

    //     glGetIntegerv(GL_STENCIL_BACK_FUNC, &_glstate_backup.stencil_back.func);
    //     glGetIntegerv(GL_STENCIL_BACK_REF, &_glstate_backup.stencil_back.ref);
    //     glGetIntegerv(GL_STENCIL_BACK_VALUE_MASK, &_glstate_backup.stencil_back.value_mask);
    //     glGetIntegerv(GL_STENCIL_BACK_WRITEMASK, &_glstate_backup.stencil_back.writemask);
    //     glGetIntegerv(GL_STENCIL_BACK_FAIL, &_glstate_backup.stencil_back.fail);
    //     glGetIntegerv(GL_STENCIL_BACK_PASS_DEPTH_FAIL, &_glstate_backup.stencil_back.pass_depth_fail);
    //     glGetIntegerv(GL_STENCIL_BACK_PASS_DEPTH_PASS, &_glstate_backup.stencil_back.pass_depth_pass);

    //     // Setup expected GL state.
    //     glViewport(0, 0, windowSize.x, windowSize.x);

    //     glClearStencil(0);
    //     glClearColor(0, 0, 0, 0);

    //     glActiveTexture(GL_TEXTURE0);

    //     glDisable(GL_SCISSOR_TEST);
    //     glDisable(GL_CULL_FACE);

    //     // Set blending function for premultiplied alpha.
    //     glEnable(GL_BLEND);
    //     glBlendEquation(GL_FUNC_ADD);
    //     glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    // #ifndef RMLUI_PLATFORM_EMSCRIPTEN
    //     // We do blending in nonlinear sRGB space because that is the common practice and gives results that we are
    //     used to. glDisable(GL_FRAMEBUFFER_SRGB);
    // #endif

    //     glEnable(GL_STENCIL_TEST);
    //     glStencilFunc(GL_ALWAYS, 1, GLuint(-1));
    //     glStencilMask(GLuint(-1));
    //     glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

    //     glDisable(GL_DEPTH_TEST);

    //     SetTransform(nullptr);

    //     render_layers.BeginFrame(viewport_width, viewport_height);
    //     glBindFramebuffer(GL_FRAMEBUFFER, render_layers.GetTopLayer().framebuffer);
    //     glClear(GL_COLOR_BUFFER_BIT);

    //     UseProgram(ProgramId::None);
    //     program_transform_dirty.set();
    //     scissor_state = Rml::Rectanglei::MakeInvalid();

    //     Gfx::CheckGLError("BeginFrame");
    // }

    // Draws the result to the backbuffer and restores OpenGL state.
    // void EndFrame()
    // {
    // }
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
