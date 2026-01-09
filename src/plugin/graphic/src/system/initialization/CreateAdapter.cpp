#include "CreateAdapter.hpp"
#include "Logger.hpp"
#include "exception/AdapterCreationError.hpp"
#include "resource/Context.hpp"
#include "resource/GraphicSettings.hpp"

#include <optional>
#include <vector>

namespace {

struct AdapterInfo {
    wgpu::Adapter adapter;
    wgpu::AdapterType type;
    wgpu::BackendType backend;
};

std::vector<AdapterInfo> EnumerateAvailableAdapters(const wgpu::Instance &instance)
{
    wgpu::InstanceEnumerateAdapterOptions enumOpts(wgpu::Default);
    size_t count = instance.enumerateAdapters(enumOpts, nullptr);
    if (count == 0)
        return {};

    std::vector<wgpu::Adapter> rawAdapters(count);
    instance.enumerateAdapters(enumOpts, rawAdapters.data());

    std::vector<AdapterInfo> result;
    result.reserve(count);

    for (auto &adapter : rawAdapters)
    {
        wgpu::AdapterInfo info(wgpu::Default);
        if (adapter.getInfo(&info) != wgpu::Status::Success)
            continue;

        result.push_back({adapter, info.adapterType, info.backendType});
    }

    return result;
}

bool IsSoftwareAdapter(const AdapterInfo &info)
{
    return info.type == wgpu::AdapterType::CPU;
}

bool IsHardwareAdapter(const AdapterInfo &info)
{
    return info.type == wgpu::AdapterType::DiscreteGPU || info.type == wgpu::AdapterType::IntegratedGPU;
}

std::optional<AdapterInfo> FindBestHardwareAdapter(const std::vector<AdapterInfo> &adapters)
{
    for (const auto &info : adapters)
    {
        if (info.type == wgpu::AdapterType::DiscreteGPU)
            return info;
    }

    for (const auto &info : adapters)
    {
        if (info.type == wgpu::AdapterType::IntegratedGPU)
            return info;
    }

    return std::nullopt;
}

std::optional<AdapterInfo> FindSoftwareAdapter(const std::vector<AdapterInfo> &adapters)
{
    for (const auto &info : adapters)
    {
        if (IsSoftwareAdapter(info))
            return info;
    }

    return std::nullopt;
}

wgpu::PowerPreference ConvertPowerPreference(Graphic::Resource::PowerPreference pref)
{
    if (pref == Graphic::Resource::PowerPreference::LowPower)
        return wgpu::PowerPreference::LowPower;

    if (pref == Graphic::Resource::PowerPreference::HighPerformance)
        return wgpu::PowerPreference::HighPerformance;

    return wgpu::PowerPreference::Undefined;
}

wgpu::Adapter RequestAdapterWithSurface(wgpu::Instance &instance, const wgpu::RequestAdapterOptions &opts)
{
    return instance.requestAdapter(opts);
}

wgpu::Adapter RequestAdapterWithoutSurface(wgpu::Instance &instance, const wgpu::RequestAdapterOptions &opts)
{
    wgpu::RequestAdapterOptions safeOpts = opts;
    safeOpts.compatibleSurface = nullptr;
    return instance.requestAdapter(safeOpts);
}

void LogSoftwareAdapterWarning()
{
    Log::Warn("Using software adapter (CPU rendering). This may cause crashes with surface operations.");
    Log::Warn("Consider setting WGPU_BACKEND=gl environment variable for better compatibility.");
}

void LogNoHardwareAdapterWarning()
{
    Log::Warn("No hardware GPU adapter found. Falling back to software rendering.");
}

} // namespace

void Graphic::System::CreateAdapter(Engine::Core &core)
{
    auto &context = core.GetResource<Resource::Context>();
    const auto &graphicSettings = core.GetResource<Resource::GraphicSettings>();

    auto availableAdapters = EnumerateAvailableAdapters(context.instance.value());
    if (availableAdapters.empty())
        throw Exception::AdapterCreationError("No WebGPU adapters available on this system");

    auto hardwareAdapter = FindBestHardwareAdapter(availableAdapters);
    auto softwareAdapter = FindSoftwareAdapter(availableAdapters);

    wgpu::RequestAdapterOptions adapterOpts(wgpu::Default);
    adapterOpts.powerPreference = ConvertPowerPreference(graphicSettings.GetPowerPreference());

    bool hasSurface = context.surface.has_value() && context.surface->value.has_value();
    bool useSoftwareOnly = !hardwareAdapter.has_value();

    wgpu::Adapter selectedAdapter;
    wgpu::BackendType selectedBackend = wgpu::BackendType::Undefined;
    bool isSoftware = false;

    if (useSoftwareOnly)
    {
        LogNoHardwareAdapterWarning();

        if (!softwareAdapter.has_value())
            throw Exception::AdapterCreationError("No suitable adapter found (neither hardware nor software)");

        LogSoftwareAdapterWarning();
        selectedAdapter = softwareAdapter->adapter;
        selectedBackend = softwareAdapter->backend;
        isSoftware = true;
    }
    else
    {
        if (hasSurface)
        {
            adapterOpts.compatibleSurface = context.surface->value.value();
        }

        wgpu::Adapter adapter = RequestAdapterWithSurface(context.instance.value(), adapterOpts);

        if (adapter == nullptr && hasSurface)
        {
            Log::Warn("Failed to get adapter with surface compatibility. Retrying without surface constraint.");
            adapter = RequestAdapterWithoutSurface(context.instance.value(), adapterOpts);
        }

        if (adapter == nullptr)
            throw Exception::AdapterCreationError("Could not get WebGPU adapter");

        wgpu::AdapterInfo info(wgpu::Default);
        if (adapter.getInfo(&info) == wgpu::Status::Success)
        {
            selectedBackend = info.backendType;
            isSoftware = (info.adapterType == wgpu::AdapterType::CPU);
        }
        selectedAdapter = adapter;
    }

    context.adapter = selectedAdapter;
    context.backendType = selectedBackend;
    // On ne considère software bloquant que si ce n'est pas OpenGL/OpenGLES
    context.isSoftwareAdapter = isSoftware && (selectedBackend != wgpu::BackendType::OpenGL && selectedBackend != wgpu::BackendType::OpenGLES);
}
