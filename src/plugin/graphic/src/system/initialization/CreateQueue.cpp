#include "system/initialization/CreateQueue.hpp"
#include "exception/QueueCreationError.hpp"
#include "resource/Context.hpp"
#include "resource/GraphicSettings.hpp"
#include "resource/Queue.hpp"
#include "utils/webgpu.hpp"

void Graphic::System::CreateQueue(Engine::Core &core)
{
    auto &context = core.GetResource<Resource::Context>();
    auto &device = context.deviceContext.GetDevice();

    if (!device.has_value())
        return;

    auto queue = device->getQueue();

    if (queue == nullptr)
        throw Exception::QueueCreationError("Could not create WebGPU queue");

    core.RegisterResource(Resource::Queue(queue));
}
