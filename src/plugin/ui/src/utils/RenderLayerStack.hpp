/*
 * The RmlUi backend is mostly taken from the official implementation of the RmlUi GLFW GL3 backend
 * See: https://github.dev/mikke89/RmlUi/blob/master/Backends/RmlUi_Renderer_GL3.h
 */

#pragma once

#include <RmlUi/Core.h>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

#include "Logger.hpp"

// Determines the anti-aliasing quality when creating layers. Enables better-looking visuals, especially when transforms
// are applied.
#ifndef RMLUI_NUM_MSAA_SAMPLES
constexpr int RMLUI_NUM_MSAA_SAMPLES = 2;
#endif

namespace ES::Plugin::UI::Utils
{
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
    RenderLayerStack() { _fbPostprocess.resize(4); }
    ~RenderLayerStack() { DestroyFramebuffers(); }
    
    enum class FramebufferAttachment {
        None,
        DepthStencil
    };

    struct FramebufferData {
        int width;
        int height;
        GLuint framebuffer;
        GLuint color_tex_buffer;
        GLuint color_render_buffer;
        GLuint depth_stencil_buffer;
        bool owns_depth_stencil_buffer;
    };

    bool CreateFramebuffer(FramebufferData &out_fb, int width, int height, int samples,
                                  FramebufferAttachment attachment, GLuint shared_depth_stencil_buffer);
    void DestroyFramebuffer(FramebufferData &fb);

    // Push a new layer. All references to previously retrieved layers are invalidated.
    Rml::LayerHandle PushLayer();

    // Pop the top layer. All references to previously retrieved layers are invalidated.
    void PopLayer();

    const FramebufferData &GetLayer(Rml::LayerHandle layer) const;
    const FramebufferData &GetTopLayer() const;
    Rml::LayerHandle GetTopLayerHandle() const;
    const FramebufferData &GetPostprocessPrimary();
    const FramebufferData &GetPostprocessSecondary();
    const FramebufferData &GetPostprocessTertiary();
    const FramebufferData &GetBlendMask();
    void SwapPostprocessPrimarySecondary();
    void BeginFrame(int new_width, int new_height);
    void EndFrame();

private:
    void DestroyFramebuffers()
    {
        RMLUI_ASSERTMSG(_layersSize == 0,
                        "Do not call this during frame rendering, that is, between BeginFrame() and EndFrame().");

        for (FramebufferData &fb : _fbLayers)
            DestroyFramebuffer(fb);

        _fbLayers.clear();

        for (FramebufferData &fb : _fbPostprocess)
            DestroyFramebuffer(fb);
    }

    const FramebufferData &EnsureFramebufferPostprocess(int index)
    {
        RMLUI_ASSERT(index < static_cast<int>(_fbPostprocess.size()))
        FramebufferData &fb = _fbPostprocess[index];
        if (!fb.framebuffer)
            CreateFramebuffer(fb, _width, _height, 0, FramebufferAttachment::None, 0);
        return fb;
    }

    // The number of active layers is manually tracked since we re-use the framebuffers stored in the fb_layers
    // stack.
    int _layersSize = 0;
    
    int _width = 0;
    int _height = 0;
    Rml::Vector<FramebufferData> _fbLayers;
    Rml::Vector<FramebufferData> _fbPostprocess;
};
} // namespace ES::Plugin::UI::Utils
