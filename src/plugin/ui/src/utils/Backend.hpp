#pragma once

#include <RmlUi/Core.h>
#include <glfw/glfw3.h>

#include "Logger.hpp"
#include "OpenGL.pch.hpp"
#include "OpenGL.hpp"
#include "Object.hpp"
#include "Mesh.hpp"
#include "Window.hpp"

namespace ES::Plugin::UI::Utils
{
class RenderInterface : public Rml::RenderInterface {
public:
    explicit RenderInterface(ES::Engine::Core &core) : _core(core) {}

private:
    ES::Engine::Core &_core;

    struct GeometryRecord {
        entt::hashed_string mesh_handle;
        Object::Component::Mesh mesh;
    };

    struct Geometry {
        GLuint vao = 0;
        GLuint vbo = 0;
        GLuint ibo = 0;
        GLsizei num_indices = 0;
    };

    std::unordered_map<Rml::CompiledGeometryHandle, GeometryRecord> _geometry_map;
    std::unordered_map<Rml::TextureHandle, entt::hashed_string> _texture_handle_map;
    
    uintptr_t _next_geometry_id = 1;
    uintptr_t _next_texture_id = 1;

public:
    Rml::CompiledGeometryHandle CompileGeometry(Rml::Span<const Rml::Vertex> vertices, Rml::Span<const int> indices) override {
        GeometryRecord record;

        auto &mesh = record.mesh;
        mesh.vertices.reserve(vertices.size());
        mesh.normals.resize(vertices.size(), glm::vec3(0.0f));
        mesh.texCoords.reserve(vertices.size());

        for (const auto &v : vertices) {
            mesh.vertices.emplace_back(v.position.x, v.position.y, 0.0f);
            mesh.texCoords.emplace_back(v.tex_coord.x, v.tex_coord.y);
        }
        mesh.indices.assign(indices.begin(), indices.end());

        auto handle_id = fmt::format("rml_mesh_{}", _next_geometry_id);
        entt::hashed_string mesh_handle = entt::hashed_string(handle_id.c_str());

        auto &bufferManager = _core.GetResource<ES::Plugin::OpenGL::Resource::GLMeshBufferManager>();
        if (!bufferManager.Contains(mesh_handle)) {
            ES::Plugin::OpenGL::Utils::GLMeshBuffer buffer;
            buffer.GenerateGLMeshBuffers(mesh);
            bufferManager.Add(mesh_handle, buffer);
        }

        record.mesh_handle = mesh_handle;
        Rml::CompiledGeometryHandle id = _next_geometry_id++;
        _geometry_map.emplace(id, std::move(record));
        return id;
    }

    void RenderGeometry(Rml::CompiledGeometryHandle handle, Rml::Vector2f translation, Rml::TextureHandle texture_handle) override {
        const auto it = _geometry_map.find(handle);
        if (it == _geometry_map.end()) return;

        const auto &mesh = it->second.mesh;
        const auto &mesh_handle = it->second.mesh_handle;

        auto &textureManager = _core.GetResource<ES::Plugin::OpenGL::Resource::TextureManager>();
        auto tex_it = _texture_handle_map.find(texture_handle);
        if (tex_it != _texture_handle_map.end()) {
            auto tex = textureManager.Get(tex_it->second);
            if (tex.IsValid()) {
                tex.Bind();
            } else {
                glBindTexture(GL_TEXTURE_2D, 0);
            }
        } else {
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        auto &bufferManager = _core.GetResource<ES::Plugin::OpenGL::Resource::GLMeshBufferManager>();
        auto &buffer = bufferManager.Get(mesh_handle);

        // glPushMatrix();
        // glTranslatef(translation.x, translation.y, 0.0f);
        buffer.Draw(mesh);
        // glPopMatrix();
    }

    void ReleaseGeometry(Rml::CompiledGeometryHandle handle) override {
        auto it = _geometry_map.find(handle);
        if (it != _geometry_map.end()) {
            const auto &mesh_handle = it->second.mesh_handle;
            auto &bufferManager = _core.GetResource<ES::Plugin::OpenGL::Resource::GLMeshBufferManager>();
            if (bufferManager.Contains(mesh_handle)) {
                bufferManager.Remove(mesh_handle);
            }
            _geometry_map.erase(it);
        }
    }

    Rml::TextureHandle LoadTexture(Rml::Vector2i &texture_dimensions, const Rml::String &source) override {
        auto texture_id = fmt::format("rml_texture_{}", _next_texture_id);
        entt::hashed_string handle = entt::hashed_string(texture_id.c_str());

        auto &textureManager = _core.GetResource<ES::Plugin::OpenGL::Resource::TextureManager>();
        if (!textureManager.Contains(handle)) {
            ES::Plugin::OpenGL::Utils::Texture tex(source.c_str());
            textureManager.Add(handle, tex);
        }

        auto &texture = textureManager.Get(handle);
        if (!texture.IsValid())
        {
            ES::Utils::Log::Error(fmt::format("Rmlui: Loaded texture {} is not valid", texture_id));
            return 0;
        }

        texture_dimensions = { texture.GetWidth(), texture.GetHeight() };

        Rml::TextureHandle id = _next_texture_id++;
        _texture_handle_map[id] = handle;
        return id;
    }

    Rml::TextureHandle GenerateTexture(Rml::Span<const Rml::byte> source, Rml::Vector2i dimensions) override {
        auto texture_id = fmt::format("rml_dynamic_texture_{}", _next_texture_id);
        entt::hashed_string handle = entt::hashed_string(texture_id.c_str());

        auto &textureManager = _core.GetResource<ES::Plugin::OpenGL::Resource::TextureManager>();
        if (!textureManager.Contains(handle)) {
            ES::Plugin::OpenGL::Utils::Texture tex(source.data(), dimensions.x, dimensions.y);
            textureManager.Add(handle, tex);
        }

        auto &texture = textureManager.Get(handle);
        if (!texture.IsValid())
        {
            ES::Utils::Log::Error(fmt::format("Rmlui: Generated texture {} is not valid", texture_id));
            return 0;
        }

        Rml::TextureHandle id = _next_texture_id++;
        _texture_handle_map[id] = handle;
        return id;
    }

    void ReleaseTexture(Rml::TextureHandle handle) override {
        auto it = _texture_handle_map.find(handle);
        if (it != _texture_handle_map.end()) {
            auto &textureManager = _core.GetResource<ES::Plugin::OpenGL::Resource::TextureManager>();
            textureManager.Remove(it->second);
            _texture_handle_map.erase(it);
        }
    }

    void EnableScissorRegion(bool enable) override {
        if (enable) glEnable(GL_SCISSOR_TEST);
        else glDisable(GL_SCISSOR_TEST);
    }

    void SetScissorRegion(Rml::Rectanglei region) override {
        glScissor(region.Left(), region.Top(), region.Width(), region.Height());
    }
};

class SystemInterface : public Rml::SystemInterface
{
    double GetElapsedTime() override {
        return glfwGetTime();
    }

    bool LogMessage(Rml::Log::Type type, const Rml::String &message) override {
        switch (type) {
            case Rml::Log::Type::LT_ASSERT:
            case Rml::Log::Type::LT_ERROR:
                ES::Utils::Log::Error(fmt::format("RmlUi: {}", message));
                break;
            case Rml::Log::Type::LT_WARNING:
                ES::Utils::Log::Warn(fmt::format("RmlUi: {}", message));
                break;
            case Rml::Log::Type::LT_INFO:
                ES::Utils::Log::Info(fmt::format("RmlUi: {}", message));
                break;
            default:
                break;
        }
        return true;
    }
};
} // namespace ES::Plugin::UI::Utils
