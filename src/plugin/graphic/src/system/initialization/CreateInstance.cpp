#include "CreateInstance.hpp"
#include "exception/InstanceCreationError.hpp"
#include "resource/Context.hpp"
#include "utils/webgpu.hpp"

namespace Plugin::Graphic::System {

void CreateInstance(Engine::Core &core)
{
    // TODO: should we print debug info like this? Could it be useful?
    Log::Debug("Creating WebGPU instance...");

    wgpu::InstanceDescriptor desc(wgpu::Default);

    wgpu::Instance instance(wgpuCreateInstance(&desc));

    if (instance == nullptr)
        throw Exception::InstanceCreationError("Could not create WebGPU instance, wgpuCreateInstance returned nullptr");

    core.GetResource<Graphic::Resource::Context>().instance = instance;

    // TODO: same question as above
    Log::Debug(fmt::format("WebGPU instance created: {}", static_cast<void *>(instance)));
}

} // namespace Plugin::Graphic::System
