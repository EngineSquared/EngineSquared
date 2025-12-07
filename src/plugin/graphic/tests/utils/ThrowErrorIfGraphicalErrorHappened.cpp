#include "utils/ThrowErrorIfGraphicalErrorHappened.hpp"
#include "resource/GraphicSettings.hpp"
#include "utils/TestGraphicalError.hpp"

namespace Graphic::Tests::Utils {

void ThrowErrorIfGraphicalErrorHappened(Engine::Core &core)
{
    core.GetResource<Graphic::Resource::GraphicSettings>().SetOnErrorCallback(
        [](WGPUDevice const *, WGPUErrorType type, WGPUStringView message, WGPU_NULLABLE void *,
           WGPU_NULLABLE void *) {
            Log::Error(fmt::format("Custom uncaptured device error: type {:x} ({})", static_cast<uint32_t>(type),
                                   std::string(message.data, message.length)));
            throw Exception::TestGraphicalError("Custom uncaptured device error occurred");
        });
}
} // namespace Graphic::Tests::Utils
