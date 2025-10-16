#include "CreateAdapter.hpp"
#include "exception/AdapterCreationError.hpp"
#include "resource/Context.hpp"
#include "resource/GraphicSettings.hpp"

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

    context.adapter = adapter;
}
