#include "system/initialization/CreateQueue.hpp"
#include "exception/QueueCreationError.hpp"
#include "resource/Context.hpp"
#include "resource/GraphicSettings.hpp"
#include "utils/webgpu.hpp"

void Plugin::Graphic::System::CreateQueue(Engine::Core &core)
{
    auto &context = core.GetResource<Resource::Context>();
    const auto &settings = core.GetResource<Resource::GraphicSettings>();

    if (settings.GetWindowSystem() == Resource::WindowSystem::None)
        return;

    auto queue = context.deviceContext.GetDevice()->getQueue();

    if (queue == nullptr)
        throw Exception::QueueCreationError("Could not create WebGPU queue");

    context.queue = queue;
}
