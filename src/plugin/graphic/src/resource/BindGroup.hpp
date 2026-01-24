#pragma once

#include <initializer_list>
#include <vector>

#include "core/Core.hpp"
#include "exception/BindGroupCreationError.hpp"
#include "resource/DeviceContext.hpp"
#include "resource/GPUBufferContainer.hpp"
#include "resource/SamplerContainer.hpp"
#include "resource/ShaderContainer.hpp"
#include "resource/TextureContainer.hpp"
#include "utils/webgpu.hpp"

namespace Graphic::Resource {

class BindGroup {
  public:
    struct Asset {
        enum class Type {
            Buffer,
            Sampler,
            Texture,
        };

        uint32_t binding;
        Type type;
        entt::hashed_string name;
        uint64_t size; // For buffer type
    };

    BindGroup(Engine::Core &core, std::string_view name, entt::hashed_string shaderId, uint32_t layoutIndex,
              std::initializer_list<Asset> assets)
        : _shaderId(shaderId), _layoutIndex(layoutIndex), _name(name), _assets(assets)
    {
        _entries = _CreateBindGroupEntries(core);
        _bindGroup = _CreateBindGroup(core);
    }

    BindGroup(const BindGroup &other) = delete;
    BindGroup(BindGroup &&other) noexcept
        : _name(std::move(other._name)), _shaderId(other._shaderId), _layoutIndex(other._layoutIndex),
          _assets(std::move(other._assets)), _entries(std::move(other._entries)), _bindGroup(other._bindGroup)
    {
        other._bindGroup = nullptr;
    }

    ~BindGroup()
    {
        if (_bindGroup)
            _bindGroup.release();
    }

    const wgpu::BindGroup &GetBindGroup() const { return _bindGroup; }

    uint32_t GetLayoutIndex() const { return _layoutIndex; }

    const std::vector<wgpu::BindGroupEntry> &GetEntries() const { return _entries; }

    void Refresh(Engine::Core &core)
    {
        _RefreshBindGroupEntries(core);
        _RefreshBindGroup(core);
    }

  private:
    void _RefreshBindGroupEntries(Engine::Core &core)
    {
        _entries.clear();
        _entries = _CreateBindGroupEntries(core);
    }

    std::vector<wgpu::BindGroupEntry> _CreateBindGroupEntries(Engine::Core &core)
    {
        std::vector<wgpu::BindGroupEntry> entries;
        entries.reserve(_assets.size());
        for (const auto &asset : _assets)
        {
            entries.push_back(_CreateBindGroupEntry(core, asset));
        }
        return entries;
    }

    void _RefreshBindGroup(Engine::Core &core)
    {
        auto newBindGroup = _CreateBindGroup(core);
        _bindGroup.release();
        _bindGroup = newBindGroup;
    }

    wgpu::BindGroup _CreateBindGroup(Engine::Core &core)
    {
        auto &shaders = core.GetResource<Graphic::Resource::ShaderContainer>();
        auto &shader = shaders.Get(_shaderId);
        auto layout = shader.GetBindGroupLayout(_layoutIndex);

        wgpu::BindGroupDescriptor descriptor(wgpu::Default);
        descriptor.layout = layout;
        descriptor.label = wgpu::StringView(_name);
        descriptor.entryCount = static_cast<uint32_t>(_entries.size());
        descriptor.entries = _entries.empty() ? nullptr : _entries.data();

        auto &deviceContext = core.GetResource<Graphic::Resource::Context>();
        auto bindGroup = deviceContext.deviceContext.GetDevice()->createBindGroup(descriptor);

        layout.release();

        if (!bindGroup)
            throw Exception::BindGroupCreationError("Failed to create BindGroup");
        return bindGroup;
    }

    wgpu::BindGroupEntry _CreateBindGroupEntry(Engine::Core &core, const Asset &asset)
    {
        wgpu::BindGroupEntry entry(wgpu::Default);
        entry.buffer = nullptr;
        entry.sampler = nullptr;
        entry.textureView = nullptr;
        entry.size = 0;
        entry.offset = 0;
        entry.binding = asset.binding;

        switch (asset.type)
        {
        case Asset::Type::Buffer: {
            auto &gpuBufferContainer = core.GetResource<Graphic::Resource::GPUBufferContainer>();
            auto &buffer = gpuBufferContainer.Get(asset.name);
            entry.buffer = buffer->GetBuffer();
            entry.size = buffer->GetBuffer().getSize();
            break;
        }
        case Asset::Type::Sampler: {
            auto &samplerContainer = core.GetResource<Graphic::Resource::SamplerContainer>();
            auto &sampler = samplerContainer.Get(asset.name);
            entry.sampler = sampler.GetSampler();
            break;
        }
        case Asset::Type::Texture: {
            auto &textureContainer = core.GetResource<Graphic::Resource::TextureContainer>();
            auto &texture = textureContainer.GetOrDefault(asset.name);
            entry.textureView = texture.GetDefaultView();
            break;
        }
        default: throw Exception::BindGroupCreationError("Unexpected Asset::Type value in _CreateBindGroupEntry");
        }

        return entry;
    }

    entt::hashed_string _shaderId;
    uint32_t _layoutIndex = 0;
    std::string _name;
    std::vector<Asset> _assets;
    std::vector<wgpu::BindGroupEntry> _entries;
    wgpu::BindGroup _bindGroup;
};

} // namespace Graphic::Resource
