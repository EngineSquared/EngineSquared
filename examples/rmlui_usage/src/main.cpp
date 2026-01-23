/**************************************************************************
 * EngineSquared - Rmlui Usage Example
 *
 * This example demonstrates how to use the Rmlui plugin.
 **************************************************************************/

#include "GLFW/glfw3.h"
#include "Logger.hpp"
#include "RmlUi/Core/Unit.h"
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
#include "scheduler/PreUpdate.hpp"
#include "scheduler/Preparation.hpp"
#include "scheduler/Startup.hpp"
#include "spdlog/fmt/bundled/format.h"
#include "webgpu.h"
#include <RmlUi/Core/Core.h>
#include <RmlUi/Core/Element.h>
#include <RmlUi/Core/Math.h>
#include <RmlUi/Core/Property.h>
#include <RmlUi/Core/StyleSheetSpecification.h>
#include <RmlUi/Core/TransformPrimitive.h>
#include <RmlUi/Core/Tween.h>
#include <RmlUi/Core/Types.h>
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

struct OverlayState {
    bool demo = true;
    bool animation = false;
    bool transform = false;
    bool demoDirty = false;
    bool animationDirty = false;
    bool transformDirty = false;
    bool animationInitialized = false;
    double animationLastFadeTime = 0.0;
};

void InitializeAnimationOverlay(Rmlui::Resource::UIContext &ui, OverlayState &state)
{
    using namespace Rml;

    if (auto *el = ui.GetElementById("start_game"))
    {
        auto p1 = Transform::MakeProperty({Transforms::Rotate2D{10.0F}, Transforms::TranslateX{100.0F}});
        auto p2 = Transform::MakeProperty({Transforms::Scale2D{3.0F}});
        el->Animate("transform", p1, 1.8F, Tween{Tween::Elastic, Tween::InOut}, -1, true);
        el->AddAnimationKey("transform", p2, 1.3F, Tween{Tween::Elastic, Tween::InOut});
    }
    if (auto *el = ui.GetElementById("high_scores"))
    {
        el->Animate("margin-left", Property(0.0F, Unit::PX), 0.3F, Tween{Tween::Sine, Tween::In}, 10, true, 1.0F);
        el->AddAnimationKey("margin-left", Property(100.0F, Unit::PX), 3.0F, Tween{Tween::Circular, Tween::Out});
    }
    if (auto *el = ui.GetElementById("options"))
    {
        el->Animate("image-color", Property(Colourb(128, 255, 255, 255), Unit::COLOUR), 0.3F, Tween{}, -1, false);
        el->AddAnimationKey("image-color", Property(Colourb(128, 128, 255, 255), Unit::COLOUR), 0.3F);
        el->AddAnimationKey("image-color", Property(Colourb(0, 128, 128, 255), Unit::COLOUR), 0.3F);
        el->AddAnimationKey("image-color", Property(Colourb(64, 128, 255, 0), Unit::COLOUR), 0.9F);
        el->AddAnimationKey("image-color", Property(Colourb(255, 255, 255, 255), Unit::COLOUR), 0.3F);
    }
    if (auto *el = ui.GetElementById("exit"))
    {
        PropertyDictionary pd;
        StyleSheetSpecification::ParsePropertyDeclaration(pd, "transform", "translate(200px, 200px) rotate(1215deg)");
        el->Animate("transform", *pd.GetProperty(PropertyId::Transform), 3.0F, Tween{Tween::Bounce, Tween::Out}, -1);
    }
    if (auto *el = ui.GetElementById("generic"))
    {
        auto p = Transform::MakeProperty({
            Transforms::TranslateY{50.0F, Unit::PX},
            Transforms::Rotate3D{0.0F, 0.0F, 1.0F, -90.0F, Unit::DEG},
            Transforms::ScaleY{0.8F}
        });
        el->Animate("transform", p, 1.5F, Tween{Tween::Sine, Tween::InOut}, -1, true);
    }
    if (auto *el = ui.GetElementById("combine"))
    {
        auto p = Transform::MakeProperty({
            Transforms::Translate2D{50.0F, 50.0F, Unit::PX},
            Transforms::Rotate2D(1215.0F)
        });
        el->Animate("transform", p, 8.0F, Tween{}, -1, true);
    }
    if (auto *el = ui.GetElementById("decomposition"))
    {
        auto p = Transform::MakeProperty({
            Transforms::TranslateY{50.0F, Unit::PX},
            Transforms::Rotate3D{0.8F, 0.0F, 1.0F, 110.0F, Unit::DEG}
        });
        el->Animate("transform", p, 1.3F, Tween{Tween::Quadratic, Tween::InOut}, -1, true);
    }
    if (auto *el = ui.GetElementById("abs_rel"))
    {
        el->Animate("margin-left", Property(50.0F, Unit::PERCENT), 1.5F, Tween{}, -1, true);
    }
    if (auto *el = ui.GetElementById("abs_rel_transform"))
    {
        auto p = Transform::MakeProperty({
            Transforms::TranslateX{0.0F, Unit::PX}
        });
        el->Animate("transform", p, 1.5F, Tween{}, -1, true);
    }
    if (auto *el = ui.GetElementById("animation_event"))
    {
        el->Animate("top", Property(Math::RandomReal(250.0F), Unit::PX), 1.5F, Tween{Tween::Cubic, Tween::InOut});
        el->Animate("left", Property(Math::RandomReal(250.0F), Unit::PX), 1.5F, Tween{Tween::Cubic, Tween::InOut});
        ui.RegisterEventListener(*el, "animationend", [](Event &event) {
            if (auto *target = event.GetTargetElement())
            {
                if (target->GetId() == "animation_event")
                {
                    target->Animate("top", Property(Math::RandomReal(200.0F), Unit::PX), 1.2F,
                                    Tween{Tween::Cubic, Tween::InOut});
                    target->Animate("left", Property(Math::RandomReal(100.0F), Unit::PERCENT), 0.8F,
                                    Tween{Tween::Cubic, Tween::InOut});
                }
            }
        });
    }
    if (auto *el = ui.GetElementById("transition_class"))
    {
        ui.RegisterEventListener(*el, "click", [](Event &event) {
            if (auto *target = event.GetTargetElement())
            {
                if (target->GetId() == "transition_class")
                {
                    target->SetClass("move_me", !target->IsClassSet("move_me"));
                }
            }
        });
    }

    state.animationInitialized = true;
    state.animationLastFadeTime = 0.0;
}

void UpdateAnimationOverlay(Engine::Core &core)
{
    auto &state = core.GetResource<OverlayState>();
    if (!state.animation || !state.animationInitialized)
    {
        return;
    }

    auto &ui = core.GetResource<Rmlui::Resource::UIContext>();
    auto *help = ui.GetElementById("help");
    if (help == nullptr)
    {
        return;
    }

    const double t = Rml::GetSystemInterface()->GetElapsedTime();
    if ((t - state.animationLastFadeTime) >= 1.4F)
    {
        if (help->IsClassSet("fadeout"))
        {
            help->SetClass("fadeout", false);
            help->SetClass("fadein", true);
        }
        else if (help->IsClassSet("fadein"))
        {
            help->SetClass("fadein", false);
            help->SetClass("textalign", true);
        }
        else
        {
            help->SetClass("textalign", false);
            help->SetClass("fadeout", true);
        }
        state.animationLastFadeTime = t;
    }
}

void ApplyOverlayState(Engine::Core &core)
{
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
        state.animationInitialized = false;
    }
    if (state.transformDirty)
    {
        state.transformDirty = false;
        if (state.transform)
            ui.LoadOverlayDocument("asset/transform.rml");
        else
            ui.UnloadOverlayDocument("asset/transform.rml");
    }

    if (state.animation && !state.animationInitialized)
    {
        InitializeAnimationOverlay(ui, state);
    }
}

void Setup(Engine::Core &core)
{
    auto &window = core.GetResource<Window::Resource::Window>();
    window.SetSize(1280, 720);

    auto camera = core.CreateEntity();

    camera.AddComponent<Object::Component::Transform>(glm::vec3(0.0F, 0.0F, -2.0F));
    camera.AddComponent<Object::Component::Camera>();

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
    core.RegisterSystem<RenderingPipeline::PreUpdate>(UpdateAnimationOverlay);

    core.RegisterSystem<RenderingPipeline::Preparation>(ApplyOverlayState);
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
