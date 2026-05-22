#include "CreateAdapter.hpp"
#include "exception/AdapterCreationError.hpp"
#include "resource/Adapter.hpp"
#include "resource/DeviceContext.hpp"
#include "resource/GraphicSettings.hpp"
#include "resource/Instance.hpp"
#include "resource/Surface.hpp"

#include <optional>
#include <vector>

static std::optional<wgpu::BackendType> getBackendType(const wgpu::Adapter &adapter)
{
    wgpu::AdapterInfo info(wgpu::Default);
    if (adapter.getInfo(&info) != wgpu::Status::Success)
    {
        info.freeMembers();
        return std::nullopt;
    }
    wgpu::BackendType backendType = info.backendType;
    info.freeMembers();
    return backendType;
}

static std::optional<wgpu::Adapter> findVulkanAdapter(const Graphic::Resource::Instance &instance)
{
    wgpu::InstanceEnumerateAdapterOptions enumOpts(wgpu::Default);
    size_t count = instance->enumerateAdapters(enumOpts, nullptr);
    if (count == 0)
        return std::nullopt;

    std::vector<wgpu::Adapter> adapters(count);
    instance->enumerateAdapters(enumOpts, adapters.data());
    for (auto &cand : adapters)
    {
        wgpu::AdapterInfo info(wgpu::Default);
        if (cand.getInfo(&info) != wgpu::Status::Success)
        {
            info.freeMembers();
            continue;
        }
        if (info.backendType == wgpu::BackendType::Vulkan)
        {
            info.freeMembers();
            adapters.data()->release();
            return cand;
        }
        info.freeMembers();
    }
    for (auto &adapter : adapters)
        adapter.release();
    return std::nullopt;
}

static void selectVulkanAdapter(const Graphic::Resource::Instance &instance, wgpu::Adapter &adapter)
{
    auto currentBackend = getBackendType(adapter);
    if (!currentBackend.has_value())
        return;

    if (currentBackend.value() == wgpu::BackendType::Vulkan)
        return;

    if (auto vulkan = findVulkanAdapter(instance))
        adapter = *vulkan;
}

void Graphic::System::CreateAdapter(Engine::Core &core)
{
    const auto &graphicSettings = core.GetResource<Resource::GraphicSettings>();

    if (!core.HasResource<Resource::Instance>())
    {
        Log::Error("Graphic::System::CreateAdapter: context.instance has no value");
        return;
    }
    auto &instance = core.GetResource<Resource::Instance>();

    wgpu::RequestAdapterOptions adapterOpts(wgpu::Default);

    if (graphicSettings.GetPowerPreference() == Resource::PowerPreference::LowPower)
    {
        adapterOpts.powerPreference = wgpu::PowerPreference::LowPower;
    }
    else if (graphicSettings.GetPowerPreference() == Resource::PowerPreference::HighPerformance)
    {
        adapterOpts.powerPreference = wgpu::PowerPreference::HighPerformance;
    }

    if (core.HasResource<Resource::Surface>())
    {
        auto &surface = core.GetResource<Resource::Surface>();
        if (surface.value.has_value())
        {
            adapterOpts.compatibleSurface = surface.value.value();
        }
    }

    wgpu::Adapter adapter = instance->requestAdapter(adapterOpts);

    if (adapter == nullptr)
        throw Exception::AdapterCreationError("Could not get WebGPU adapter");

    selectVulkanAdapter(instance, adapter);

    core.RegisterResource(Resource::Adapter(adapter));
}
