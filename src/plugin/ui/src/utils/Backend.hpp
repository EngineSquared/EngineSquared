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

// Determines the anti-aliasing quality when creating layers. Enables better-looking visuals, especially when transforms
// are applied.
#ifndef RMLUI_NUM_MSAA_SAMPLES
#    define RMLUI_NUM_MSAA_SAMPLES 2
#endif

namespace ES::Plugin::UI::Utils {
class RenderInterface : public Rml::RenderInterface {
  public:
    RenderInterface() = delete;
    explicit RenderInterface(ES::Engine::Core &core);
    ~RenderInterface();

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

    ES::Engine::Core &_core;
    // // Rml::CompiledGeometryHandle _fullScreenSquad = {};
    glm::mat4 _transformMatrix;
    std::unordered_map<Rml::CompiledGeometryHandle, CompiledGeometryData> _geometries;
    std::unordered_map<Rml::TextureHandle, TextureData> _textures;

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

    void UseShaderProgram(const entt::hashed_string &program_id);
    void DisableActiveShaderProgram();

    /// Flip the vertical axis of the rectangle, and move its origin to the vertically opposite side of the viewport.
    /// @note Changes the coordinate system from RmlUi to OpenGL, or equivalently in reverse.
    /// @note The Rectangle::Top and Rectangle::Bottom members will have reverse meaning in the returned rectangle.
    Rml::Rectanglei VerticallyFlipped(Rml::Rectanglei rect, int viewport_height);

    static bool CreateFramebuffer(FramebufferData &out_fb, int width, int height, int samples,
                                  FramebufferAttachment attachment, GLuint shared_depth_stencil_buffer);
    static void DestroyFramebuffer(FramebufferData &fb);

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

class RmlEventBackend : public Rml::EventListener {
public:
    using EventCallback = std::function<void(const std::string &event_name, const std::string &element_id)>;

private:
    ES::Engine::Core &_core;
    Rml::Context *_context;
    EventCallback _event_callback;

    static int ConvertKeyModifiers(int glfw_mods) {
        int key_modifier_state = 0;
        if (GLFW_MOD_SHIFT & glfw_mods) key_modifier_state |= Rml::Input::KM_SHIFT;
        if (GLFW_MOD_CONTROL & glfw_mods) key_modifier_state |= Rml::Input::KM_CTRL;
        if (GLFW_MOD_ALT & glfw_mods) key_modifier_state |= Rml::Input::KM_ALT;
        if (GLFW_MOD_CAPS_LOCK & glfw_mods) key_modifier_state |= Rml::Input::KM_SCROLLLOCK;
        if (GLFW_MOD_NUM_LOCK & glfw_mods) key_modifier_state |= Rml::Input::KM_NUMLOCK;
        return key_modifier_state;
    }

public:
    RmlEventBackend() = delete;
    RmlEventBackend(ES::Engine::Core &core, Rml::Context &context) : _core(core), _context(&context) {}

    void SetCallback() {
        auto &inputManager = _core.GetResource<ES::Plugin::Input::Resource::InputManager>();
        inputManager.RegisterMouseButtonCallback([this](ES::Engine::Core &, int key, int action, int mods) {
            ProcessMouseButton(key, action, mods);
        });
    }

    void ProcessMouseButton(int button, int action, int mods) {
        if (_context) {
            switch (action) {
                case GLFW_PRESS:
                    _context->ProcessMouseButtonDown(button, ConvertKeyModifiers(mods));
                    break;
                case GLFW_RELEASE:
                    _context->ProcessMouseButtonUp(button, ConvertKeyModifiers(mods));
                    break;
            }
        }
    }

    void ProcessEvent(Rml::Event &event) override {
        if (_event_callback) {
            const std::string tag = event.GetCurrentElement()->GetId();
            _event_callback(event.GetType(), tag);
        }
    }

    void AttachEvents(const std::string &eventType, Rml::Element &toElement) {
        toElement.AddEventListener(eventType, this);
    }

    void SetEventCallback(EventCallback callback) {
        _event_callback = std::move(callback);
    }
};
} // namespace ES::Plugin::UI::Utils
