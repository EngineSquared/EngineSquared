#pragma once

#include <RmlUi/Core.h>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>

#include "RmlUi/Config/Config.h"
#include "RmlUi/Core/Colour.h"
#include "RmlUi/Core/Math.h"
#include "RmlUi/Core/Span.h"
#include "RmlUi/Core/Types.h"
#include "RmlUi/Core/Vertex.h"

#include "core/Core.hpp"
#include "resource/Texture.hpp"

#include "utils/webgpu.hpp"
#include "utils/IRenderer.hpp"

namespace Rmlui::Utils
{
    class RenderInterface : public Rmlui::Utils::IRenderer
    {
    public:
        RenderInterface() = delete;
        explicit RenderInterface(Engine::Core &core);
        ~RenderInterface() override = default;

        static RenderInterface *GetActive();
        void FlushDrawCommands(wgpu::RenderPassEncoder const &renderPass);

        Rml::CompiledGeometryHandle CompileGeometry(Rml::Span<const Rml::Vertex> vertices, Rml::Span<const int> indices) override;
        void RenderGeometry(Rml::CompiledGeometryHandle handle, Rml::Vector2f translation, Rml::TextureHandle texture_handle) override;
        void ReleaseGeometry(Rml::CompiledGeometryHandle handle) override;
        Rml::TextureHandle LoadTexture(Rml::Vector2i &texture_dimensions, const Rml::String &source) override;
        Rml::TextureHandle GenerateTexture(Rml::Span<const Rml::byte> source, Rml::Vector2i dimensions) override;
        Rml::TextureHandle CreateTexture(Rml::Span<const Rml::byte> source_data, Rml::Vector2i source_dimensions);
        void ReleaseTexture(Rml::TextureHandle handle) override;
        void EnableScissorRegion(bool enable) override;
        void SetScissorRegion(Rml::Rectanglei region) override;
        void SetScissor(Rml::Rectanglei region, bool vertically_flip);
        void BeginFrame() override;
        void EndFrame() override;
        void DrawFullscreenQuad();
        void SetTransform(const Rml::Matrix4f *new_transform) override;
    
    private:
        struct GeometryData
        {
            std::vector<Rml::Vertex> vertices;
            std::vector<int> indices;
        };

        struct TextureData
        {
            Rml::Vector2i size;
            std::vector<Rml::byte> pixels;
            std::unique_ptr<Graphic::Resource::Texture> gpuTexture;
            wgpu::Sampler sampler;
            wgpu::BindGroup bindGroup;
        };

        struct DrawCommand
        {
            wgpu::Buffer vertexBuffer;
            wgpu::Buffer indexBuffer;
            uint32_t indexCount = 0;
            wgpu::BindGroup textureBindGroup;
            wgpu::BindGroup screenBindGroup;
            bool scissorEnabled = false;
            Rml::Rectanglei scissorRegion;
        };

        static RenderInterface *_active;

        Engine::Core &_core;
        std::unordered_map<Rml::CompiledGeometryHandle, std::unique_ptr<GeometryData>> _geometries;
        std::unordered_map<Rml::TextureHandle, std::unique_ptr<TextureData>> _textures;
        size_t _textureCounter = 0;
        std::vector<DrawCommand> _drawCommands;
        std::unique_ptr<TextureData> _defaultTexture;
        wgpu::Buffer _screenBuffer;
        wgpu::BindGroup _screenBindGroup;
        bool _scissorEnabled = false;
        Rml::Rectanglei _scissorRegion;
    };
} // namespace Rmlui::Utils
