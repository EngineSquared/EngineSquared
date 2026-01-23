/**
 * EngineSquared - Rmlui Demo Sample
 */

#include "DemoCommon.hpp"
#include "Logger.hpp"
#include "RmlUi/Config/Config.h"
#include "RmlUi/Core/ID.h"
#include "RmlUi/Core/Input.h"
#include "RmlUi/Core/Property.h"
#include "RmlUi/Core/StyleTypes.h"
#include "RmlUi/Core/Traits.h"
#include "RmlUi/Core/Transform.h"
#include "RmlUi/Core/Tween.h"
#include "RmlUi/Core/Types.h"
#include "RmlUi/Core/Unit.h"
#include "core/Core.hpp"
#include "plugin/PluginCameraMovement.hpp"
#include "plugin/PluginDefaultPipeline.hpp"
#include "plugin/PluginInput.hpp"
#include "plugin/PluginRmlui.hpp"
#include "plugin/PluginWindow.hpp"
#include "resource/GraphicSettings.hpp"
#include "resource/UIContext.hpp"
#include "scheduler/Init.hpp"
#include "scheduler/PreUpdate.hpp"
#include "scheduler/Startup.hpp"
#include "spdlog/fmt/bundled/format.h"
#include "webgpu.h"

#include <RmlUi/Core/Core.h>
#include <RmlUi/Core/Elements/ElementFormControl.h>
#include <RmlUi/Core/EventListener.h>
#include <RmlUi/Core/EventListenerInstancer.h>
#include <RmlUi/Core/Factory.h>
#include <RmlUi/Core/FileInterface.h>
#include <RmlUi/Core/Math.h>
#include <RmlUi/Core/StreamMemory.h>
#include <RmlUi/Core/StringUtilities.h>
#include <RmlUi/Core/StyleSheetContainer.h>
#include <RmlUi/Core/TransformPrimitive.h>

#include <cstdint>
#include <cstdlib>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

namespace demo_setup {
static const Rml::String kSandboxDefaultRcss = R"(
body { top: 0; left: 0; right: 0; bottom: 0; overflow: hidden auto; }
scrollbarvertical { width: 15px; }
scrollbarvertical slidertrack { background: #eee; }
scrollbarvertical slidertrack:active { background: #ddd; }
scrollbarvertical sliderbar { width: 15px; min-height: 30px; background: #aaa; }
scrollbarvertical sliderbar:hover { background: #888; }
scrollbarvertical sliderbar:active { background: #666; }
scrollbarhorizontal { height: 15px; }
scrollbarhorizontal slidertrack { background: #eee; }
scrollbarhorizontal slidertrack:active { background: #ddd; }
scrollbarhorizontal sliderbar { height: 15px; min-width: 30px; background: #aaa; }
scrollbarhorizontal sliderbar:hover { background: #888; }
scrollbarhorizontal sliderbar:active { background: #666; }
)";

struct TweeningParameters {
    Rml::Tween::Type type = Rml::Tween::Linear;
    Rml::Tween::Direction direction = Rml::Tween::Out;
    float duration = 0.5F;
};

class DemoWindow : public Rml::EventListener {
  public:
    bool Initialize(Engine::Core &core, Rml::ElementDocument &doc)
    {
        document = &doc;
        context = doc.GetContext();
        engineCore = &core;

        if (auto *titleElement = document->GetElementById("title"))
        {
            titleElement->SetInnerRML("Demo sample");
        }

        if (auto *source =
                rmlui_dynamic_cast<Rml::ElementFormControl *>(document->GetElementById("sandbox_rml_source")))
        {
            auto value = source->GetValue();
            value += "<p>Write your RML here</p>\n\n<!-- <img src=\"asset/high_scores_alien_1.tga\"/> -->";
            source->SetValue(value);
        }

        if (auto *target = document->GetElementById("sandbox_target"))
        {
            iframe = context->CreateDocument();
            auto iframePtr = iframe->GetParentNode()->RemoveChild(iframe);
            target->AppendChild(std::move(iframePtr));
            iframe->SetProperty(Rml::PropertyId::Position, Rml::Property(Rml::Style::Position::Absolute));
            iframe->SetProperty(Rml::PropertyId::Display, Rml::Property(Rml::Style::Display::Block));
            iframe->SetInnerRML("<p>Rendered output goes here.</p>");

            Rml::String styleSheetContent;
            auto *fileInterface = Rml::GetFileInterface();
            Rml::FileHandle handle = fileInterface->Open("asset/rml.rcss");
            if (handle != 0U)
            {
                size_t length = fileInterface->Length(handle);
                styleSheetContent.resize(length);
                fileInterface->Read(reinterpret_cast<void *>(styleSheetContent.data()), length, handle);
                fileInterface->Close(handle);
                styleSheetContent += kSandboxDefaultRcss;
            }
            else
            {
                styleSheetContent = kSandboxDefaultRcss;
            }

            Rml::StreamMemory stream(reinterpret_cast<Rml::byte *>(styleSheetContent.data()),
                                     styleSheetContent.size());
            stream.SetSourceURL("sandbox://default_rcss");

            rmlBasicStyleSheet = Rml::MakeShared<Rml::StyleSheetContainer>();
            rmlBasicStyleSheet->LoadStyleSheetContainer(&stream);
        }

        if (auto *source =
                rmlui_dynamic_cast<Rml::ElementFormControl *>(document->GetElementById("sandbox_rcss_source")))
        {
            Rml::String value =
                "/* Write your RCSS here */\n\n/* body { color: #fea; background: #224; }\nimg { image-color: red; } */";
            source->SetValue(value);
            SetSandboxStylesheet(value);
        }

        gauge = document->GetElementById("gauge");
        progressHorizontal = document->GetElementById("progress_horizontal");

        return true;
    }

    void Update()
    {
        if (iframe != nullptr)
        {
            iframe->UpdateDocument();
        }

        if (submitting && (gauge != nullptr) && (progressHorizontal != nullptr))
        {
            constexpr float kProgressbarsTime = 2.0F;
            const float progress =
                Rml::Math::Min(static_cast<float>(Rml::GetSystemInterface()->GetElapsedTime() - submittingStartTime) /
                                   kProgressbarsTime,
                               2.0F);

            float valueGauge = 1.0F;
            float valueHorizontal = 0.0F;
            if (progress < 1.0F)
            {
                valueGauge = 0.5F - 0.5F * Rml::Math::Cos(Rml::Math::RMLUI_PI * progress);
            }
            else
            {
                valueHorizontal = 0.5F - 0.5F * Rml::Math::Cos(Rml::Math::RMLUI_PI * (progress - 1.0F));
            }

            progressHorizontal->SetAttribute("value", valueHorizontal);

            const float valueBegin = 0.09F;
            const float valueEnd = 1.0F - valueBegin;
            float valueMapped = valueBegin + (valueGauge * (valueEnd - valueBegin));
            gauge->SetAttribute("value", valueMapped);

            auto valueGaugeStr = Rml::CreateString("%d %%", Rml::Math::RoundToInteger(valueGauge * 100.F));
            auto valueHorizontalStr = Rml::CreateString("%d %%", Rml::Math::RoundToInteger(valueHorizontal * 100.F));

            if (auto *elValue = document->GetElementById("gauge_value"))
            {
                elValue->SetInnerRML(valueGaugeStr);
            }
            if (auto *elValue = document->GetElementById("progress_value"))
            {
                elValue->SetInnerRML(valueHorizontalStr);
            }

            Rml::String label = "Placing tubes";
            size_t numDots = (static_cast<size_t>(progress * 10.F) % 4);
            if (progress > 1.0F)
            {
                label += "... Placed! Assembling message";
            }
            if (progress < 2.0F)
            {
                label += Rml::String(numDots, '.');
            }
            else
            {
                label += "... Done!";
            }

            if (auto *elLabel = document->GetElementById("progress_label"))
            {
                elLabel->SetInnerRML(label);
            }

            if (progress >= 2.0F)
            {
                submitting = false;
                if (auto *elOutput = document->GetElementById("form_output"))
                {
                    elOutput->SetInnerRML(submitMessage);
                }
            }

            document->GetContext()->RequestNextUpdate(.0);
        }
    }

    void ProcessEvent(Rml::Event &event) override
    {
        if (event.GetId() == Rml::EventId::Keydown && (engineCore != nullptr))
        {
            const auto keyIdentifier =
                static_cast<Rml::Input::KeyIdentifier>(event.GetParameter<int>("key_identifier", 0));
            if (keyIdentifier == Rml::Input::KI_ESCAPE)
            {
                engineCore->Stop();
            }
        }
    }

    void SubmitForm(const Rml::String &inSubmitMessage)
    {
        submitting = true;
        submittingStartTime = Rml::GetSystemInterface()->GetElapsedTime();
        submitMessage = inSubmitMessage;
        if (auto *elOutput = document->GetElementById("form_output"))
        {
            elOutput->SetInnerRML("");
        }
        if (auto *elProgress = document->GetElementById("submit_progress"))
        {
            elProgress->SetProperty("display", "block");
        }
    }

    void SetSandboxStylesheet(const Rml::String &string)
    {
        if ((iframe != nullptr) && rmlBasicStyleSheet)
        {
            auto style = Rml::MakeShared<Rml::StyleSheetContainer>();
            Rml::StreamMemory stream(reinterpret_cast<const Rml::byte *>(string.data()), string.size());
            stream.SetSourceURL("sandbox://rcss");

            style->LoadStyleSheetContainer(&stream);
            style = rmlBasicStyleSheet->CombineStyleSheetContainer(*style);
            iframe->SetStyleSheetContainer(style);
        }
    }

    void SetSandboxBody(const Rml::String &string)
    {
        if (iframe != nullptr)
        {
            iframe->SetInnerRML(string);
        }
    }

    [[nodiscard]] TweeningParameters GetTweeningParameters() const { return tweeningParameters; }

    void SetTweeningParameters(TweeningParameters inTweeningParameters) { tweeningParameters = inTweeningParameters; }

  private:
    Engine::Core *engineCore = nullptr;
    Rml::Context *context = nullptr;
    Rml::ElementDocument *document = nullptr;
    Rml::ElementDocument *iframe = nullptr;
    Rml::Element *gauge = nullptr;
    Rml::Element *progressHorizontal = nullptr;
    Rml::SharedPtr<Rml::StyleSheetContainer> rmlBasicStyleSheet;

    bool submitting = false;
    double submittingStartTime = 0;
    Rml::String submitMessage;

    TweeningParameters tweeningParameters;
};

class DemoEventListener : public Rml::EventListener {
  public:
    DemoEventListener(const Rml::String &value, Rml::Element *element, DemoWindow *demoWindow)
        : value(value), element(element), demoWindow(demoWindow)
    {
    }

    void ProcessEvent(Rml::Event &event) override
    {
        if ((demoWindow == nullptr) || (element == nullptr))
        {
            return;
        }

        if (value == "change_color")
        {
            const TweeningParameters tweeningParameters = demoWindow->GetTweeningParameters();
            const Rml::Colourb color(static_cast<Rml::byte>(Rml::Math::RandomInteger(255)),
                                     static_cast<Rml::byte>(Rml::Math::RandomInteger(255)),
                                     static_cast<Rml::byte>(Rml::Math::RandomInteger(255)));

            element->Animate("image-color", Rml::Property(color, Rml::Unit::COLOUR), tweeningParameters.duration,
                             Rml::Tween(tweeningParameters.type, tweeningParameters.direction));

            event.StopPropagation();
        }
        else if (value == "move_child")
        {
            const Rml::Vector2f mousePos = {event.GetParameter("mouse_x", 0.0F), event.GetParameter("mouse_y", 0.0F)};
            if (Rml::Element *child = element->GetFirstChild())
            {
                Rml::Vector2f newPos =
                    mousePos - element->GetAbsoluteOffset() -
                    Rml::Vector2f(0.35F * child->GetClientWidth(), 0.9F * child->GetClientHeight());
                Rml::Property destination = Rml::Transform::MakeProperty({Rml::Transforms::Translate2D(newPos.x, newPos.y)});

                const TweeningParameters tweeningParameters = demoWindow->GetTweeningParameters();
                if (tweeningParameters.duration <= 0)
                {
                    child->SetProperty(Rml::PropertyId::Transform, destination);
                }
                else
                {
                    child->Animate("transform", destination, tweeningParameters.duration,
                                   Rml::Tween(tweeningParameters.type, tweeningParameters.direction));
                }
            }
        }
        else if (value == "tween_function")
        {
            static const Rml::SmallUnorderedMap<Rml::String, Rml::Tween::Type> tweeningFunctions = {
                {"back", Rml::Tween::Back},
                {"bounce", Rml::Tween::Bounce},
                {"circular", Rml::Tween::Circular},
                {"cubic", Rml::Tween::Cubic},
                {"elastic", Rml::Tween::Elastic},
                {"exponential", Rml::Tween::Exponential},
                {"linear", Rml::Tween::Linear},
                {"quadratic", Rml::Tween::Quadratic},
                {"quartic", Rml::Tween::Quartic},
                {"quintic", Rml::Tween::Quintic},
                {"sine", Rml::Tween::Sine},
            };

            const Rml::String value = event.GetParameter("value", Rml::String());
            auto it = tweeningFunctions.find(value);
            if (it != tweeningFunctions.end())
            {
                TweeningParameters tweeningParameters = demoWindow->GetTweeningParameters();
                tweeningParameters.type = it->second;
                demoWindow->SetTweeningParameters(tweeningParameters);
            }
            else
            {
                Log::Error("Unknown tween function");
            }
        }
        else if (value == "tween_direction")
        {
            const Rml::String value = event.GetParameter("value", Rml::String());
            TweeningParameters tweeningParameters = demoWindow->GetTweeningParameters();
            if (value == "in")
            {
                tweeningParameters.direction = Rml::Tween::In;
            }
            else if (value == "out")
            {
                tweeningParameters.direction = Rml::Tween::Out;
            }
            else if (value == "in-out")
            {
                tweeningParameters.direction = Rml::Tween::InOut;
            }
            else
            {
                Log::Error("Unknown tween direction");
            }
            demoWindow->SetTweeningParameters(tweeningParameters);
        }
        else if (value == "tween_duration")
        {
            const auto value =
                static_cast<float>(std::atof(rmlui_static_cast<Rml::ElementFormControl *>(element)->GetValue().c_str()));

            TweeningParameters tweeningParameters = demoWindow->GetTweeningParameters();
            tweeningParameters.duration = value;
            demoWindow->SetTweeningParameters(tweeningParameters);

            if (auto *elDuration = element->GetElementById("duration"))
            {
                elDuration->SetInnerRML(Rml::CreateString("%2.2f", value));
            }
        }
        else if (value == "rating")
        {
            auto *elRating = element->GetElementById("rating");
            auto *elRatingEmoji = element->GetElementById("rating_emoji");
            if ((elRating != nullptr) && (elRatingEmoji != nullptr))
            {
                enum { Sad, Mediocre, Exciting, Celebrate, Champion, CountEmojis };
                static const char *emojis[CountEmojis] = {"😢", "😐", "😮", "😎", "🏆"};
                int value = event.GetParameter("value", 50);

                Rml::String emoji;
                if (value <= 0)
                {
                    emoji = emojis[Sad];
                }
                else if (value < 50)
                {
                    emoji = emojis[Mediocre];
                }
                else if (value < 75)
                {
                    emoji = emojis[Exciting];
                }
                else if (value < 100)
                {
                    emoji = emojis[Celebrate];
                }
                else
                {
                    emoji = emojis[Champion];
                }

                elRating->SetInnerRML(Rml::CreateString("%d%%", value));
                elRatingEmoji->SetInnerRML(emoji);
            }
        }
        else if (value == "submit_form")
        {
            const auto &params = event.GetParameters();
            Rml::String output = "<p>";
            for (const auto &entry : params)
            {
                auto escaped = Rml::StringUtilities::EncodeRml(entry.second.Get<Rml::String>());
                if (entry.first == "message")
                {
                    escaped = "<br/>" + escaped;
                }
                output += "<strong>" + entry.first + "</strong>: " + escaped + "<br/>";
            }
            output += "</p>";

            demoWindow->SubmitForm(output);
        }
        else if (value == "set_sandbox_body")
        {
            if (auto *source = rmlui_dynamic_cast<Rml::ElementFormControl *>(element->GetElementById("sandbox_rml_source")))
            {
                auto value = source->GetValue();
                demoWindow->SetSandboxBody(value);
            }
        }
        else if (value == "set_sandbox_style")
        {
            if (auto *source =
                    rmlui_dynamic_cast<Rml::ElementFormControl *>(element->GetElementById("sandbox_rcss_source")))
            {
                auto value = source->GetValue();
                demoWindow->SetSandboxStylesheet(value);
            }
        }
        else if (value == "cancel_selection_on_escape")
        {
            if (event.GetParameter("key_identifier", 0) == Rml::Input::KeyIdentifier::KI_ESCAPE)
            {
                event.StopPropagation();
            }
        }
    }

    void OnDetach(Rml::Element * /*element*/) override { delete this; }

  private:
    Rml::String value;
    Rml::Element *element = nullptr;
    DemoWindow *demoWindow = nullptr;
};

class DemoEventListenerInstancer : public Rml::EventListenerInstancer {
  public:
    explicit DemoEventListenerInstancer(DemoWindow *demoWindow) : demoWindow(demoWindow) {}

    Rml::EventListener *InstanceEventListener(const Rml::String &value, Rml::Element *element) override
    {
        return new DemoEventListener(value, element, demoWindow);
    }

  private:
    DemoWindow *demoWindow = nullptr;
};

struct DemoState {
    DemoWindow window;
    DemoEventListenerInstancer instancer;

    DemoState() : instancer(&window) {}
};

class RmluiExampleError : public std::runtime_error {
  public:
    using std::runtime_error::runtime_error;
};

void UpdateDemoWindow(Engine::Core &core)
{
    if (core.HasResource<DemoState>())
    {
        core.GetResource<DemoState>().window.Update();
    }
}

void Setup(Engine::Core &core)
{
    RmluiUsage::Demo::ConfigureDefaultWindowAndCamera(core);

    auto &rmluiContext = core.GetResource<Rmlui::Resource::UIContext>();
    RmluiUsage::Demo::LoadDefaultFonts(rmluiContext);

    auto &state = core.RegisterResource<DemoState>(DemoState{});
    Rml::Factory::RegisterEventListenerInstancer(&state.instancer);

    rmluiContext.LoadDocument("asset/demo.rml");
    auto *document = rmluiContext.GetDocument();
    if (document == nullptr)
    {
        throw RmluiExampleError("Failed to load demo document");
    }

    state.window.Initialize(core, *document);
    document->AddEventListener(Rml::EventId::Keydown, &state.window);
    document->AddEventListener(Rml::EventId::Keyup, &state.window);
    document->AddEventListener(Rml::EventId::Animationend, &state.window);

    RmluiUsage::Demo::AttachHoverOverlay(rmluiContext, "Demo");
    rmluiContext.EnableDebugger(true);
    core.RegisterSystem(RmluiUsage::Demo::EscapeKeySystem);
    core.RegisterSystem<RenderingPipeline::PreUpdate>(UpdateDemoWindow);
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
