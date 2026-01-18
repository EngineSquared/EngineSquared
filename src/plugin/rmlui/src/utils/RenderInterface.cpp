#include "utils/RenderInterface.hpp"
#include "resource/TextureContainer.hpp"
#include "utils/DefaultSampler.hpp"
#include "utils/RmluiRenderPass.hpp"
#include "utils/webgpu.hpp"

#include "Logger.hpp"
#include "core/Core.hpp"
#include "fmt/format.h"
#include "glm/ext/vector_uint4_sized.hpp"
#include "spdlog/fmt/bundled/format.h"

#include "RmlUi/Config/Config.h"
#include "RmlUi/Core/Span.h"
#include "RmlUi/Core/Types.h"
#include "RmlUi/Core/Vertex.h"

#include "resource/Context.hpp"
#include "resource/Image.hpp"
#include "resource/SamplerContainer.hpp"
#include "resource/ShaderContainer.hpp"
#include "resource/Texture.hpp"
#include "resource/Window.hpp"
#include "utils/EndRenderTexture.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <filesystem>
#include <glm/vec4.hpp>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace Rmlui::Utils {
namespace {
void UpdateScreenBuffer(Engine::Core &core, wgpu::Buffer const &buffer)
{
    const auto &window = core.GetResource<Window::Resource::Window>();
    const auto size = window.GetSize();
    const std::array<float, 4> data = {static_cast<float>(size.x), static_cast<float>(size.y), 0.0F, 0.0F};
    const auto &context = core.GetResource<Graphic::Resource::Context>();
    context.queue.value().writeBuffer(buffer, 0, data.data(), sizeof(data));
}

wgpu::BindGroup CreateTextureBindGroup(Engine::Core &core, const wgpu::BindGroupLayout &layout,
                                       const Graphic::Resource::Texture &texture, const wgpu::Sampler &sampler)
{
    wgpu::BindGroupEntry entries[2] = {};
    entries[0].binding = 0;
    entries[0].textureView = texture.GetDefaultView();
    entries[1].binding = 1;
    entries[1].sampler = sampler;

    wgpu::BindGroupDescriptor descriptor(wgpu::Default);
    descriptor.layout = layout;
    descriptor.entryCount = 2;
    descriptor.entries = entries;

    const auto &context = core.GetResource<Graphic::Resource::Context>();
    return context.deviceContext.GetDevice()->createBindGroup(descriptor);
}
} // namespace

RenderInterface *RenderInterface::_active = nullptr;
RenderInterface::RenderInterface(Engine::Core &core) : _core(core) {}

RenderInterface *RenderInterface::GetActive() { return _active; }

void RenderInterface::FlushDrawCommands(wgpu::RenderPassEncoder const &renderPass)
{
    auto renderSize = _core.GetResource<Window::Resource::Window>().GetSize();
    if (_core.HasResource<Graphic::Resource::TextureContainer>())
    {
        auto &textures = _core.GetResource<Graphic::Resource::TextureContainer>();
        if (textures.Contains(Graphic::Utils::END_RENDER_TEXTURE_ID))
        {
            const auto &endTexture = textures.Get(Graphic::Utils::END_RENDER_TEXTURE_ID);
            const auto textureSize = endTexture.GetSize();
            renderSize.x = textureSize.x;
            renderSize.y = textureSize.y;
        }
    }
    for (auto &command : _drawCommands)
    {
        if (command.textureBindGroup != nullptr)
        {
            renderPass.setBindGroup(0, command.textureBindGroup, 0, nullptr);
        }
        if (command.screenBindGroup != nullptr)
        {
            renderPass.setBindGroup(1, command.screenBindGroup, 0, nullptr);
        }
        renderPass.setVertexBuffer(0, command.vertexBuffer, 0, command.vertexBuffer.getSize());
        renderPass.setIndexBuffer(command.indexBuffer, wgpu::IndexFormat::Uint32, 0, command.indexBuffer.getSize());
        if (command.scissorEnabled)
        {
            const int left = std::max(0, command.scissorRegion.Left());
            const int top = std::max(0, command.scissorRegion.Top());
            const int right = std::min(command.scissorRegion.Right(), static_cast<int>(renderSize.x));
            const int bottom = std::min(command.scissorRegion.Bottom(), static_cast<int>(renderSize.y));

            const auto x = static_cast<uint32_t>(left);
            const auto y = static_cast<uint32_t>(top);
            const auto width = static_cast<uint32_t>(std::max(0, right - left));
            const auto height = static_cast<uint32_t>(std::max(0, bottom - top));
            renderPass.setScissorRect(x, y, width, height);
        }
        else
        {
            renderPass.setScissorRect(0, 0, renderSize.x, renderSize.y);
        }
        renderPass.drawIndexed(command.indexCount, 1, 0, 0, 0);

        command.vertexBuffer.release();
        command.indexBuffer.release();
    }
    _drawCommands.clear();
}

Rml::CompiledGeometryHandle RenderInterface::CompileGeometry(Rml::Span<const Rml::Vertex> vertices,
                                                             Rml::Span<const int> indices)
{
    auto geometry = std::make_unique<GeometryData>();
    geometry->vertices.assign(vertices.begin(), vertices.end());
    geometry->indices.assign(indices.begin(), indices.end());

    const auto handle = reinterpret_cast<Rml::CompiledGeometryHandle>(geometry.get());
    _geometries.emplace(handle, std::move(geometry));
    return handle;
}

void RenderInterface::RenderGeometry(Rml::CompiledGeometryHandle handle, Rml::Vector2f translation,
                                     Rml::TextureHandle texture_handle)
{
    auto geometryIt = _geometries.find(handle);
    if (geometryIt == _geometries.end())
    {
        return;
    }

    auto const &geometry = *geometryIt->second;
    if (geometry.vertices.empty() || geometry.indices.empty())
    {
        return;
    }

    const auto &context = _core.GetResource<Graphic::Resource::Context>();
    const auto &device = context.deviceContext.GetDevice().value();
    const auto &queue = context.queue.value();

    std::vector<Rml::Vertex> translatedVertices = geometry.vertices;
    for (auto &vertex : translatedVertices)
    {
        const float pos_x = vertex.position.x + translation.x;
        const float pos_y = vertex.position.y + translation.y;

        if (_transform.has_value())
        {
            const auto &matrix = *_transform;
            const float pos_z = 0.0F;
            const float pos_w = 1.0F;
            const float transformed_x = (matrix[0][0] * pos_x) + (matrix[1][0] * pos_y) + (matrix[2][0] * pos_z) +
                                        (matrix[3][0] * pos_w);
            const float transformed_y = (matrix[0][1] * pos_x) + (matrix[1][1] * pos_y) + (matrix[2][1] * pos_z) +
                                        (matrix[3][1] * pos_w);
            const float transformed_w = (matrix[0][3] * pos_x) + (matrix[1][3] * pos_y) + (matrix[2][3] * pos_z) +
                                        (matrix[3][3] * pos_w);

            if (transformed_w != 0.0F)
            {
                vertex.position.x = transformed_x / transformed_w;
                vertex.position.y = transformed_y / transformed_w;
            }
            else
            {
                vertex.position.x = transformed_x;
                vertex.position.y = transformed_y;
            }
        }
        else
        {
            vertex.position.x = pos_x;
            vertex.position.y = pos_y;
        }
    }

    std::vector<uint32_t> indices;
    indices.reserve(geometry.indices.size());
    for (const auto &index : geometry.indices)
    {
        indices.push_back(static_cast<uint32_t>(index));
    }

    wgpu::BufferDescriptor vertexDesc(wgpu::Default);
    vertexDesc.size = translatedVertices.size() * sizeof(Rml::Vertex);
    vertexDesc.usage = wgpu::BufferUsage::Vertex | wgpu::BufferUsage::CopyDst;
    wgpu::Buffer vertexBuffer = device.createBuffer(vertexDesc);
    queue.writeBuffer(vertexBuffer, 0, translatedVertices.data(), vertexDesc.size);

    wgpu::BufferDescriptor indexDesc(wgpu::Default);
    indexDesc.size = indices.size() * sizeof(uint32_t);
    indexDesc.usage = wgpu::BufferUsage::Index | wgpu::BufferUsage::CopyDst;
    wgpu::Buffer indexBuffer = device.createBuffer(indexDesc);
    queue.writeBuffer(indexBuffer, 0, indices.data(), indexDesc.size);

    wgpu::BindGroup textureBindGroup = nullptr;
    if (texture_handle != 0UL)
    {
        auto textureIt = _textures.find(texture_handle);
        if (textureIt != _textures.end())
        {
            auto &texture = *textureIt->second;
            if (texture.bindGroup == nullptr)
            {
                const auto &shaders = _core.GetResource<Graphic::Resource::ShaderContainer>();
                if (shaders.Contains(Rmlui::Utils::RMLUI_RENDER_PASS_SHADER_ID) && texture.gpuTexture &&
                    (texture.sampler != nullptr))
                {
                    const auto &shader = shaders.Get(Rmlui::Utils::RMLUI_RENDER_PASS_SHADER_ID);
                    texture.bindGroup = CreateTextureBindGroup(_core, shader.GetBindGroupLayout(0), *texture.gpuTexture,
                                                               texture.sampler);
                }
            }
            textureBindGroup = texture.bindGroup;
        }
    }

    if (textureBindGroup == nullptr && _defaultTexture)
    {
        textureBindGroup = _defaultTexture->bindGroup;
    }

    if (textureBindGroup == nullptr || _screenBindGroup == nullptr)
    {
        return;
    }

    DrawCommand cmd;
    cmd.vertexBuffer = vertexBuffer;
    cmd.indexBuffer = indexBuffer;
    cmd.indexCount = static_cast<uint32_t>(indices.size());
    cmd.textureBindGroup = textureBindGroup;
    cmd.screenBindGroup = _screenBindGroup;
    cmd.scissorEnabled = _scissorEnabled;
    cmd.scissorRegion = _scissorRegion;
    _drawCommands.push_back(cmd);
}

void RenderInterface::ReleaseGeometry(Rml::CompiledGeometryHandle handle) { _geometries.erase(handle); }

Rml::TextureHandle RenderInterface::LoadTexture(Rml::Vector2i &texture_dimensions, const Rml::String &source)
{
    if (source.empty())
    {
        texture_dimensions = Rml::Vector2i(0, 0);
        Log::Warn("Rmlui texture source is empty.");
        return 0;
    }

    try
    {
        Graphic::Resource::Image image{std::filesystem::path(source)};
        texture_dimensions = Rml::Vector2i(static_cast<int>(image.width), static_cast<int>(image.height));

        const auto *bytes = reinterpret_cast<const Rml::byte *>(image.pixels.data());
        const size_t size_in_bytes = image.pixels.size() * sizeof(image.pixels[0]);
        return CreateTexture(Rml::Span<const Rml::byte>(bytes, size_in_bytes), texture_dimensions);
    }
    catch (const std::exception &error)
    {
        texture_dimensions = Rml::Vector2i(0, 0);
        Log::Warn(fmt::format("Rmlui failed to load texture '{}': {}", source, error.what()));
        return 0;
    }
}

Rml::TextureHandle RenderInterface::GenerateTexture(Rml::Span<const Rml::byte> source, Rml::Vector2i dimensions)
{
    return CreateTexture(source, dimensions);
}

Rml::TextureHandle RenderInterface::CreateTexture(Rml::Span<const Rml::byte> source_data,
                                                  Rml::Vector2i source_dimensions)
{
    if (source_dimensions.x <= 0 || source_dimensions.y <= 0)
    {
        Log::Warn("Rmlui texture dimensions are invalid.");
        return 0;
    }

    auto texture = std::make_unique<TextureData>();
    texture->size = source_dimensions;
    texture->pixels.assign(source_data.begin(), source_data.end());

    Graphic::Resource::Image image;
    image.width = static_cast<uint32_t>(source_dimensions.x);
    image.height = static_cast<uint32_t>(source_dimensions.y);
    image.channels = 4UL;
    image.pixels.resize(static_cast<size_t>(image.width) * static_cast<size_t>(image.height));

    if (!texture->pixels.empty())
    {
        const size_t pixel_count = image.pixels.size();
        const size_t expected_bytes = pixel_count * 4UL;
        if (texture->pixels.size() >= expected_bytes)
        {
            for (size_t i = 0UL; i < pixel_count; ++i)
            {
                const size_t offset = i * 4UL;
                image.pixels[i] = glm::u8vec4(texture->pixels[offset + 0UL], texture->pixels[offset + 1UL],
                                              texture->pixels[offset + 2UL], texture->pixels[offset + 3UL]);
            }
        }
        else
        {
            Log::Warn("Rmlui texture data size does not match expected RGBA pixel count.");
        }
    }

    const auto &context = _core.GetResource<Graphic::Resource::Context>();
    const std::string textureName = fmt::format("rmlui_texture_{}", _textureCounter);

    _textureCounter += 1UL;
    texture->gpuTexture = std::make_unique<Graphic::Resource::Texture>(context, textureName, image);

    if (const auto &samplers = _core.GetResource<Graphic::Resource::SamplerContainer>();
        samplers.Contains(Graphic::Utils::DEFAULT_SAMPLER_ID))
    {
        texture->sampler = samplers.Get(Graphic::Utils::DEFAULT_SAMPLER_ID).GetSampler();
    }

    if (const auto &shaders = _core.GetResource<Graphic::Resource::ShaderContainer>();
        shaders.Contains(Rmlui::Utils::RMLUI_RENDER_PASS_SHADER_ID))
    {
        const auto &shader = shaders.Get(Rmlui::Utils::RMLUI_RENDER_PASS_SHADER_ID);
        const auto &layout = shader.GetBindGroupLayout(0U);
        if ((texture->sampler != nullptr) && texture->gpuTexture)
        {
            texture->bindGroup = CreateTextureBindGroup(_core, layout, *texture->gpuTexture, texture->sampler);
        }
    }

    const auto textureHandle = reinterpret_cast<Rml::TextureHandle>(texture.get());
    _textures.emplace(textureHandle, std::move(texture));
    return textureHandle;
}

void RenderInterface::ReleaseTexture(Rml::TextureHandle textureHandle) { _textures.erase(textureHandle); }

void RenderInterface::EnableScissorRegion(bool enable) { _scissorEnabled = enable; }

void RenderInterface::SetScissorRegion(Rml::Rectanglei region) { _scissorRegion = region; }

void RenderInterface::SetScissor(Rml::Rectanglei region, bool vertically_flip)
{
    (void) vertically_flip; // TODO: Need to handle it
    _scissorRegion = region;
}

void RenderInterface::BeginFrame()
{
    _active = this;
    _drawCommands.clear();

    const auto &context = _core.GetResource<Graphic::Resource::Context>();
    const auto &device = context.deviceContext.GetDevice().value();

    if (_screenBuffer == nullptr)
    {
        wgpu::BufferDescriptor bufferDesc(wgpu::Default);
        bufferDesc.size = sizeof(float) * 4UL;
        bufferDesc.usage = wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst;
        _screenBuffer = device.createBuffer(bufferDesc);
    }
    UpdateScreenBuffer(_core, _screenBuffer);

    const auto &shaders = _core.GetResource<Graphic::Resource::ShaderContainer>();
    if (shaders.Contains(Rmlui::Utils::RMLUI_RENDER_PASS_SHADER_ID))
    {
        const auto &shader = shaders.Get(Rmlui::Utils::RMLUI_RENDER_PASS_SHADER_ID);
        const auto screenLayout = shader.GetBindGroupLayout(1);

        wgpu::BindGroupEntry entry(wgpu::Default);
        entry.binding = 0U;
        entry.buffer = _screenBuffer;
        entry.size = _screenBuffer.getSize();

        wgpu::BindGroupDescriptor descriptor(wgpu::Default);
        descriptor.layout = screenLayout;
        descriptor.entryCount = 1UL;
        descriptor.entries = &entry;

        _screenBindGroup = device.createBindGroup(descriptor);

        if (!_defaultTexture)
        {
            std::array<Rml::byte, 4> whitePixel = {255, 255, 255, 255};
            _defaultTexture = std::make_unique<TextureData>();
            _defaultTexture->size = Rml::Vector2i(1, 1);
            _defaultTexture->pixels.assign(whitePixel.begin(), whitePixel.end());

            Graphic::Resource::Image image;
            image.width = 1;
            image.height = 1;
            image.channels = 4;
            image.pixels = {glm::u8vec4(255, 255, 255, 255)};
            _defaultTexture->gpuTexture = std::make_unique<Graphic::Resource::Texture>(context, "rmlui_white", image);

            const auto &samplers = _core.GetResource<Graphic::Resource::SamplerContainer>();
            if (samplers.Contains(Graphic::Utils::DEFAULT_SAMPLER_ID))
            {
                _defaultTexture->sampler = samplers.Get(Graphic::Utils::DEFAULT_SAMPLER_ID).GetSampler();
                _defaultTexture->bindGroup = CreateTextureBindGroup(
                    _core, shader.GetBindGroupLayout(0), *_defaultTexture->gpuTexture, _defaultTexture->sampler);
            }
        }
    }
}

void RenderInterface::EndFrame()
{
    // Keep active renderer for render pass execution.
}

void RenderInterface::DrawFullscreenQuad() { /* TODO: Implement */ }

void RenderInterface::SetTransform(const Rml::Matrix4f *new_transform)
{
    if (new_transform != nullptr)
    {
        _transform = *new_transform;
    }
    else
    {
        _transform.reset();
    }
}
} // namespace Rmlui::Utils
