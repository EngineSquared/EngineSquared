#pragma once

#include <initializer_list>
#include <vector>

#include "core/Core.hpp"
#include "resource/DeviceContext.hpp"
#include "resource/ShaderContainer.hpp"
#include "resource/TextureContainer.hpp"
#include "resource/GPUBufferManager.hpp"
#include "utils/webgpu.hpp"

namespace Graphic::Resource {

class BindGroup {
  public:
    struct Asset {
        uint32_t binding;
        enum class Type {
            Buffer,
            Sampler,
            Texture,
        } type;
        entt::hashed_string name;
        uint64_t size; // For buffer type
    };

    BindGroup(Engine::Core &core, entt::hashed_string shaderId, uint32_t layoutIndex,
              std::initializer_list<Asset> assets)
        : _shaderId(shaderId), _layoutIndex(layoutIndex), _assets(assets)
    {
        _entries = _CreateBindGroupEntries(core);
        _bindGroup = _CreateBindGroup(core);
    }

    wgpu::BindGroup GetBindGroup() const { return _bindGroup; }

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
        _bindGroup.release();
        _bindGroup = _CreateBindGroup(core);
    }

    wgpu::BindGroup _CreateBindGroup(Engine::Core &core)
    {
        auto &shaders = core.GetResource<Graphic::Resource::ShaderContainer>();
        auto &shader = shaders.Get(_shaderId);
        auto layout = shader.GetBindGroupLayout(_layoutIndex);

        wgpu::BindGroupDescriptor descriptor(wgpu::Default);
        descriptor.layout = layout;
        descriptor.entryCount = static_cast<uint32_t>(_entries.size());
        descriptor.entries = _entries.empty() ? nullptr : _entries.data();

        auto &deviceContext = core.GetResource<Graphic::Resource::Context>();
        auto bindGroup = deviceContext.deviceContext.GetDevice()->createBindGroup(descriptor);

        if (!bindGroup)
            throw std::runtime_error("Failed to create BindGroup"); // TODO: throw a dedicated error
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
            entry.size = asset.size;
            break;
        }
        case Asset::Type::Sampler: {
            // TODO: Implement sampler binding
            break;
        }
        case Asset::Type::Texture: {
            auto &textureContainer = core.GetResource<Graphic::Resource::TextureContainer>();
            auto &texture = textureContainer.Get(asset.name);
            entry.textureView = texture.GetDefaultView();
            break;
        }
        }

        return entry;
    }

    entt::hashed_string _shaderId;
    uint32_t _layoutIndex = 0;
    std::vector<Asset> _assets;
    std::vector<wgpu::BindGroupEntry> _entries;
    wgpu::BindGroup _bindGroup;
};

} // namespace Graphic::Resource
