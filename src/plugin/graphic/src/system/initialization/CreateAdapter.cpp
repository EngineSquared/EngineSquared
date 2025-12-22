#include "CreateAdapter.hpp"
#include "exception/AdapterCreationError.hpp"
#include "resource/Context.hpp"
#include "resource/GraphicSettings.hpp"

#include <optional>
#include <vector>

static std::optional<wgpu::BackendType> getBackendType(const wgpu::Adapter &adapter)
{
    wgpu::AdapterInfo info(wgpu::Default);
    if (adapter.getInfo(&info) != wgpu::Status::Success) return std::nullopt;
    return info.backendType;
}

static std::optional<wgpu::Adapter> findVulkanAdapter(const Graphic::Resource::Context &context)
{
    wgpu::InstanceEnumerateAdapterOptions enumOpts(wgpu::Default);
    size_t count = context.instance->enumerateAdapters(enumOpts, nullptr);
    if (count == 0) return std::nullopt;

    std::vector<wgpu::Adapter> adapters(count);
    context.instance->enumerateAdapters(enumOpts, adapters.data());
    for (auto &cand : adapters)
    {
        wgpu::AdapterInfo info(wgpu::Default);
        if (cand.getInfo(&info) != wgpu::Status::Success) continue;
        if (info.backendType == wgpu::BackendType::Vulkan) return cand;
    }
    return std::nullopt;
}

static void selectVulkanAdapter(const Graphic::Resource::Context &context, wgpu::Adapter &adapter)
{
    auto currentBackend = getBackendType(adapter);
    if (!currentBackend.has_value()) return;

    if (currentBackend.value() == wgpu::BackendType::Vulkan) return;

    if (auto vulkan = findVulkanAdapter(context))
        adapter = *vulkan;
}

void Graphic::System::CreateAdapter(Engine::Core &core)
{
    auto &context = core.GetResource<Resource::Context>();
    const auto &graphicSettings = core.GetResource<Resource::GraphicSettings>();

    wgpu::RequestAdapterOptions adapterOpts(wgpu::Default);

    if (graphicSettings.GetPowerPreference() == Resource::PowerPreference::LowPower)
    {
        adapterOpts.powerPreference = wgpu::PowerPreference::LowPower;
    }
    else if (graphicSettings.GetPowerPreference() == Resource::PowerPreference::HighPerformance)
    {
        adapterOpts.powerPreference = wgpu::PowerPreference::HighPerformance;
    }

    if (context.surface.has_value())
        adapterOpts.compatibleSurface = context.surface->value.value();

    wgpu::Adapter adapter = context.instance->requestAdapter(adapterOpts);

    if (adapter == nullptr)
        throw Exception::AdapterCreationError("Could not get WebGPU adapter");

    selectVulkanAdapter(context, adapter);

    context.adapter = adapter;
}
