#include "CreateInstance.hpp"
#include "resource/Context.hpp"
#include "utils/webgpu.hpp"

namespace Plugin::Graphic::System {

void CreateInstance(Engine::Core &core)
{
    // TODO: should we print debug info like this? Could it be useful?
    Log::Debug("Creating WebGPU instance...");

    wgpu::InstanceDescriptor desc(wgpu::Default);

    wgpu::Instance instance(wgpuCreateInstance(&desc));

    // TODO: should we throw a custom exception or a generic one like below?
    if (instance == nullptr)
        throw std::runtime_error("Could not create WebGPU instance");

    core.GetResource<Graphic::Resource::Context>().instance = instance;

    // TODO: same question as above
    Log::Debug(fmt::format("WebGPU instance created: {}", static_cast<void *>(instance)));
}

} // namespace Plugin::Graphic::System
