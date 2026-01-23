/**
 * EngineSquared - Rmlui Demo Sample
 */

#include "DemoCommon.hpp"
#include "Logger.hpp"
#include "core/Core.hpp"
#include "plugin/PluginCameraMovement.hpp"
#include "plugin/PluginDefaultPipeline.hpp"
#include "plugin/PluginInput.hpp"
#include "plugin/PluginRmlui.hpp"
#include "plugin/PluginWindow.hpp"
#include "resource/GraphicSettings.hpp"
#include "resource/UIContext.hpp"
#include "scheduler/Init.hpp"
#include "scheduler/Startup.hpp"
#include "spdlog/fmt/bundled/format.h"
#include "webgpu.h"

#include <cstdint>
#include <stdexcept>
#include <string>

namespace demo_setup {
class RmluiExampleError : public std::runtime_error {
  public:
    using std::runtime_error::runtime_error;
};

void Setup(Engine::Core &core)
{
    RmluiUsage::Demo::ConfigureDefaultWindowAndCamera(core);

    auto &rmluiContext = core.GetResource<Rmlui::Resource::UIContext>();
    RmluiUsage::Demo::LoadDefaultFonts(rmluiContext);
    rmluiContext.LoadDocument("asset/demo/data/demo.rml");
    RmluiUsage::Demo::AttachHoverOverlay(rmluiContext, "Demo");
    rmluiContext.EnableDebugger(true);
    core.RegisterSystem(RmluiUsage::Demo::EscapeKeySystem);
}
} // namespace demo_setup

int main(void)
{
    Engine::Core core;

    core.AddPlugins<Window::Plugin, DefaultPipeline::Plugin, Input::Plugin, CameraMovement::Plugin, Rmlui::Plugin>();
    core.RegisterSystem<RenderingPipeline::Init>([](Engine::Core &core) {
        core.GetResource<Graphic::Resource::GraphicSettings>().SetOnErrorCallback(
            [](WGPUDevice const *, WGPUErrorType type, WGPUStringView message, WGPU_NULLABLE void *,
               WGPU_NULLABLE void *) {
                Log::Error(fmt::format("Custom uncaptured device error: type {:x} ({})", static_cast<uint32_t>(type),
                                       std::string(message.data, message.length)));
                throw demo_setup::RmluiExampleError("Custom uncaptured device error occurred");
            });
    });

    core.RegisterSystem<Engine::Scheduler::Startup>(demo_setup::Setup);

    core.RunCore();

    return 0;
}
