/**************************************************************************
 * EngineSquared - Rmlui Usage Example
 *
 * This example demonstrates how to use the Rmlui plugin.
 **************************************************************************/

#include "GLFW/glfw3.h"
#include "Logger.hpp"
#include "component/Camera.hpp"
#include "component/Transform.hpp"
#include "core/Core.hpp"
#include "glm/ext/vector_float3.hpp"
#include "plugin/PluginCameraMovement.hpp"
#include "plugin/PluginDefaultPipeline.hpp"
#include "plugin/PluginInput.hpp"
#include "plugin/PluginRmlui.hpp"
#include "plugin/PluginWindow.hpp"
#include "resource/CameraManager.hpp"
#include "resource/GraphicSettings.hpp"
#include "resource/InputManager.hpp"
#include "resource/UIContext.hpp"
#include "resource/Window.hpp"
#include "scheduler/Init.hpp"
#include "scheduler/Preparation.hpp"
#include "scheduler/Startup.hpp"
#include "spdlog/fmt/bundled/format.h"
#include "webgpu.h"
#include <cstdint>
#include <stdexcept>
#include <string>

void EscapeKeySystem(Engine::Core &core)
{
    auto &inputManager = core.GetResource<Input::Resource::InputManager>();

    if (inputManager.IsKeyPressed(GLFW_KEY_ESCAPE))
    {
        core.Stop();
    }
}

void Setup(Engine::Core &core)
{
    auto &window = core.GetResource<Window::Resource::Window>();
    window.SetSize(1280, 720);

    auto camera = core.CreateEntity();

    camera.AddComponent<Object::Component::Transform>(core, glm::vec3(0.0F, 0.0F, -2.0F));
    camera.AddComponent<Object::Component::Camera>(core);

    auto &cameraManager = core.GetResource<CameraMovement::Resource::CameraManager>();
    cameraManager.SetActiveCamera(camera);

    auto &rmluiContext = core.GetResource<Rmlui::Resource::UIContext>();

    rmluiContext.SetFont("asset/LatoLatin-Regular.ttf");
    rmluiContext.SetFont("asset/LatoLatin-Bold.ttf");
    rmluiContext.SetFont("asset/LatoLatin-Italic.ttf");
    rmluiContext.SetFont("asset/LatoLatin-BoldItalic.ttf");
    rmluiContext.SetFont("asset/NotoEmoji-Regular.ttf");
    rmluiContext.EnableDebugger(true);
    rmluiContext.LoadDocument("asset/hover_overlay.rml");

    struct OverlayState {
        bool demo = true;
        bool animation = false;
        bool transform = false;
        bool demoDirty = false;
        bool animationDirty = false;
        bool transformDirty = false;
    };

    auto &overlayState = core.RegisterResource<OverlayState>(OverlayState{});
    rmluiContext.LoadOverlayDocument("asset/demo.rml");

    if (auto *hoverLogo = rmluiContext.GetElementById("hover-logo-demo"))
    {
        rmluiContext.RegisterEventListener(*hoverLogo, "click", [&overlayState](auto &) {
            Log::Info("Demo overlay toggled");
            overlayState.demo = !overlayState.demo;
            overlayState.demoDirty = true;
        });
    }
    if (auto *hoverLogo = rmluiContext.GetElementById("hover-logo-animation"))
    {
        rmluiContext.RegisterEventListener(*hoverLogo, "click", [&overlayState](auto &) {
            Log::Info("Animation overlay toggled");
            overlayState.animation = !overlayState.animation;
            overlayState.animationDirty = true;
        });
    }
    if (auto *hoverLogo = rmluiContext.GetElementById("hover-logo-transform"))
    {
        rmluiContext.RegisterEventListener(*hoverLogo, "click", [&overlayState](auto &) {
            Log::Info("Transform overlay toggled");
            overlayState.transform = !overlayState.transform;
            overlayState.transformDirty = true;
        });
    }
    core.RegisterSystem(EscapeKeySystem);

    core.RegisterSystem<RenderingPipeline::Preparation>([](Engine::Core &core) {
        auto &state = core.GetResource<OverlayState>();
        auto &ui = core.GetResource<Rmlui::Resource::UIContext>();

        if (state.demoDirty)
        {
            state.demoDirty = false;
            if (state.demo)
                ui.LoadOverlayDocument("asset/demo.rml");
            else
                ui.UnloadOverlayDocument("asset/demo.rml");
        }
        if (state.animationDirty)
        {
            state.animationDirty = false;
            if (state.animation)
                ui.LoadOverlayDocument("asset/animation.rml");
            else
                ui.UnloadOverlayDocument("asset/animation.rml");
        }
        if (state.transformDirty)
        {
            state.transformDirty = false;
            if (state.transform)
                ui.LoadOverlayDocument("asset/transform.rml");
            else
                ui.UnloadOverlayDocument("asset/transform.rml");
        }
    });
}

class RmluiExampleError : public std::runtime_error {
  public:
    using std::runtime_error::runtime_error;
};

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
                throw RmluiExampleError("Custom uncaptured device error occurred");
            });
    });

    core.RegisterSystem<Engine::Scheduler::Startup>(Setup);

    core.RunCore();

    return 0;
}
