#include "system/initialization/SetupQueue.hpp"
#include "utils/webgpu.hpp"
#include "resource/Context.hpp"
#include "resource/GraphicSettings.hpp"

void Plugin::Graphic::System::SetupQueue(Engine::Core &core)
{
    const auto &settings = core.GetResource<Resource::GraphicSettings>();

    if (settings.GetWindowSystem() == Resource::WindowSystem::None)
        return;

    auto &context = core.GetResource<Resource::Context>();

	auto onQueueWorkDone = [](WGPUQueueWorkDoneStatus status, WGPU_NULLABLE void* userdata1, WGPU_NULLABLE void* userdata2) {
		Log::Debug(fmt::format("Queued work finished with status: {:x}", static_cast<uint32_t>(status)));
	};

	wgpu::QueueWorkDoneCallbackInfo callbackInfo(wgpu::Default);
	callbackInfo.callback = onQueueWorkDone;
	context.queue->onSubmittedWorkDone(callbackInfo);
}
