/*
 * The RmlUi backend is mostly taken from the official implementation of the RmlUi GLFW GL3 backend
 * See: https://github.dev/mikke89/RmlUi/blob/master/Backends/RmlUi_Renderer_GL3.h
 */

#pragma once

#include <RmlUi/Core.h>
#include <RmlUi/Core/Event.h>
#include <glfw/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

#include "Logger.hpp"
#include "Mesh.hpp"
#include "Object.hpp"
#include "OpenGL.hpp"
#include "Window.hpp"
#include "InputManager.hpp"

#include "RenderLayerStack.hpp"

namespace ES::Plugin::UI::Utils
{
class RenderInterface : public Rml::RenderInterface
{
public:
    RenderInterface() = delete;
    explicit RenderInterface(ES::Engine::Core &core);
    ~RenderInterface() = default;

private:
    struct CompiledGeometryData {
        GLuint vao;
        GLuint vbo;
        GLuint ibo;
        GLsizei draw_count;
    };
    struct TextureData {
        Rml::Vector2f translation;
        Rml::Vector2i size;
        std::string key;
    };
    struct GLStateBackup {
        bool enable_cull_face;
        bool enable_blend;
        bool enable_stencil_test;
        bool enable_scissor_test;
        bool enable_depth_test;

        std::array<int, 4> viewport;
        std::array<int, 4> scissor;
        // int viewport[4];
        // int scissor[4];

        int active_texture;

        int stencil_clear_value;
        std::array<float, 4> color_clear_value;
        std::array<unsigned char, 4> color_writemask;
        // float color_clear_value[4];
        // unsigned char color_writemask[4];

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
#pragma pack()

    ES::Engine::Core &_core;
    glm::mat4 _transformMatrix;
    std::unordered_map<Rml::CompiledGeometryHandle, CompiledGeometryData> _geometries;
    std::unordered_map<Rml::TextureHandle, TextureData> _textures;

    Rml::CompiledGeometryHandle _next_geom_id = 1u;
    Rml::TextureHandle _next_tex_id = 1u;

    GLStateBackup _glstate_backup = {};
    RenderLayerStack _render_layers;
    Rml::Rectanglei _scissor_state;
    entt::hashed_string _activeShaderProgram = "";

    enum class VertexAttribute : GLuint {
        Position,
        Color0,
        TexCoord0,
        Count
    };

    static constexpr Rml::TextureHandle TextureEnableWithoutBinding = Rml::TextureHandle(-1);
    static constexpr Rml::TextureHandle TexturePostprocess = Rml::TextureHandle(-2);

    void UseShaderProgram(const entt::hashed_string &program_id);
    void DisableActiveShaderProgram();

    /// Flip the vertical axis of the rectangle, and move its origin to the vertically opposite side of the viewport.
    /// @note Changes the coordinate system from RmlUi to OpenGL, or equivalently in reverse.
    /// @note The Rectangle::Top and Rectangle::Bottom members will have reverse meaning in the returned rectangle.
    Rml::Rectanglei VerticallyFlipped(Rml::Rectanglei rect, int viewport_height);

  public:
    Rml::CompiledGeometryHandle CompileGeometry(Rml::Span<const Rml::Vertex> vertices,
                                                Rml::Span<const int> indices) override;
    void RenderGeometry(Rml::CompiledGeometryHandle handle, Rml::Vector2f translation,
                        Rml::TextureHandle texture_handle) override;
    void ReleaseGeometry(Rml::CompiledGeometryHandle handle) override;
    Rml::TextureHandle LoadTexture(Rml::Vector2i &texture_dimensions, const Rml::String &source) override;
    Rml::TextureHandle GenerateTexture(Rml::Span<const Rml::byte> source, Rml::Vector2i dimensions) override;
    Rml::TextureHandle CreateTexture(Rml::Span<const Rml::byte> source_data, Rml::Vector2i source_dimensions);
    void ReleaseTexture(Rml::TextureHandle handle) override;
    void EnableScissorRegion(bool enable) override;
    void SetScissorRegion(Rml::Rectanglei region) override;
    void SetScissor(Rml::Rectanglei region, bool vertically_flip);
    void BeginFrame();
    void EndFrame();
    void DrawFullscreenQuad();
    void SetTransform(const Rml::Matrix4f *new_transform) override;
    const glm::mat4 &GetTransform() const;
};
} // namespace ES::Plugin::UI::Utils
