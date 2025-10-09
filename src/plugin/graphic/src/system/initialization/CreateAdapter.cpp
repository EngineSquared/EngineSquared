#include "CreateAdapter.hpp"
// #include "resource/Window.hpp"
#include "resource/Context.hpp"
// #include "resource/Surface.hpp"
#include "resource/GraphicSettings.hpp"

void Plugin::Graphic::System::CreateAdapter(Engine::Core &core) {
	auto &context = core.GetResource<Resource::Context>();
	auto &graphicSettings = core.GetResource<Resource::GraphicSettings>();

	wgpu::RequestAdapterOptions adapterOpts(wgpu::Default);

    if (graphicSettings.GetPowerPreference() == Resource::PowerPreference::LowPower) {
        adapterOpts.powerPreference = wgpu::PowerPreference::LowPower;
    } else if (graphicSettings.GetPowerPreference() == Resource::PowerPreference::HighPerformance) {
        adapterOpts.powerPreference = wgpu::PowerPreference::HighPerformance;
    }

    if (context.surface.has_value())
	    adapterOpts.compatibleSurface = context.surface->value.value();

	wgpu::Adapter adapter = context.instance->requestAdapter(adapterOpts);

	if (adapter == nullptr) throw std::runtime_error("Could not get WebGPU adapter");
}
