#include "utils/ThrowErrorIfGraphicalErrorHappened.hpp"
#include "resource/GraphicSettings.hpp"

namespace Graphic::Tests::Utils {

void ThrowErrorIfGraphicalErrorHappened(Engine::Core &core)
{
    core.GetResource<Graphic::Resource::GraphicSettings>().SetOnErrorCallback(
        [](WGPUDevice const *device, WGPUErrorType type, WGPUStringView message, WGPU_NULLABLE void *userdata1,
           WGPU_NULLABLE void *userdata2) {
            Log::Error(fmt::format("Custom uncaptured device error: type {:x} ({})", static_cast<uint32_t>(type),
                                   std::string(message.data, message.length)));
            throw std::runtime_error("Custom uncaptured device error occurred");
        });
}
} // namespace Graphic::Tests::Utils
