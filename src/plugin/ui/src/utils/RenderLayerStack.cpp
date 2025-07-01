#include "RenderLayerStack.hpp"

bool ES::Plugin::UI::Utils::RenderLayerStack::CreateFramebuffer(FramebufferData &out_fb, int width, int height,
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
        glTexImage2D(GL_TEXTURE_2D, 0, color_format, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

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

    return true;
}

void ES::Plugin::UI::Utils::RenderLayerStack::DestroyFramebuffer(FramebufferData &fb)
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

Rml::LayerHandle ES::Plugin::UI::Utils::RenderLayerStack::PushLayer()
{
    RMLUI_ASSERT(_layersSize <= static_cast<int>(_fbLayers.size()));

    if (_layersSize == static_cast<int>(_fbLayers.size()))
    {
        // All framebuffers should share a single stencil buffer.
        GLuint shared_depth_stencil = (_fbLayers.empty() ? 0 : _fbLayers.front().depth_stencil_buffer);

        _fbLayers.push_back(FramebufferData{});
        CreateFramebuffer(_fbLayers.back(), _width, _height, RMLUI_NUM_MSAA_SAMPLES,
                          FramebufferAttachment::DepthStencil, shared_depth_stencil);
    }

    _layersSize += 1;
    return GetTopLayerHandle();
}

void ES::Plugin::UI::Utils::RenderLayerStack::PopLayer()
{
    RMLUI_ASSERT(_layersSize > 0);
    _layersSize -= 1;
}

const ES::Plugin::UI::Utils::RenderLayerStack::FramebufferData &
ES::Plugin::UI::Utils::RenderLayerStack::GetLayer(Rml::LayerHandle layer) const
{
    RMLUI_ASSERT((size_t) layer < (size_t) _layersSize);
    return _fbLayers[layer];
}

const ES::Plugin::UI::Utils::RenderLayerStack::FramebufferData &
ES::Plugin::UI::Utils::RenderLayerStack::GetTopLayer() const
{
    return GetLayer(GetTopLayerHandle());
}

Rml::LayerHandle ES::Plugin::UI::Utils::RenderLayerStack::GetTopLayerHandle() const
{
    RMLUI_ASSERT(_layersSize > 0);
    return static_cast<Rml::LayerHandle>(_layersSize - 1);
}

const ES::Plugin::UI::Utils::RenderLayerStack::FramebufferData &
ES::Plugin::UI::Utils::RenderLayerStack::GetPostprocessPrimary()
{
    return EnsureFramebufferPostprocess(0);
}

const ES::Plugin::UI::Utils::RenderLayerStack::FramebufferData &
ES::Plugin::UI::Utils::RenderLayerStack::GetPostprocessSecondary()
{
    return EnsureFramebufferPostprocess(1);
}

const ES::Plugin::UI::Utils::RenderLayerStack::FramebufferData &
ES::Plugin::UI::Utils::RenderLayerStack::GetPostprocessTertiary()
{
    return EnsureFramebufferPostprocess(2);
}

const ES::Plugin::UI::Utils::RenderLayerStack::FramebufferData &ES::Plugin::UI::Utils::RenderLayerStack::GetBlendMask()
{
    return EnsureFramebufferPostprocess(3);
}

void ES::Plugin::UI::Utils::RenderLayerStack::SwapPostprocessPrimarySecondary()
{
    std::swap(_fbPostprocess[0], _fbPostprocess[1]);
}

void ES::Plugin::UI::Utils::RenderLayerStack::BeginFrame(int new_width, int new_height)
{
    RMLUI_ASSERT(_layersSize == 0);

    if (new_width != _width || new_height != _height)
    {
        _width = new_width;
        _height = new_height;

        DestroyFramebuffers();
    }

    PushLayer();
}

void ES::Plugin::UI::Utils::RenderLayerStack::EndFrame()
{
    RMLUI_ASSERT(_layersSize == 1);
    PopLayer();
}
