#include "system/BindInputCallbacks.hpp"

#include "Logger.hpp"
#include "core/Core.hpp"
#include "resource/InputManager.hpp"
#include "resource/UIContext.hpp"

void Rmlui::System::BindInputCallbacks(Engine::Core &core)
{
    auto &uiContext = core.GetResource<Rmlui::Resource::UIContext>();
    if (uiContext.AreInputCallbacksRegistered())
    {
        Log::Warn("Rmlui input callbacks already registered for this core");
        return;
    }
    uiContext.SetInputCallbacksRegistered(true);

    auto &input = core.GetResource<Input::Resource::InputManager>();

    Rmlui::Resource::UIContext::InputCallbackIds callbackIds;

    callbackIds.keyCallbackId =
        input.RegisterKeyCallback([](Engine::Core &context, int key, int scancode, int action, int mods) {
            (void) scancode;
            context.GetResource<Rmlui::Resource::UIContext>().ProcessKey(key, action, mods);
        });

    callbackIds.charCallbackId = input.RegisterCharCallback([](Engine::Core &context, unsigned int codepoint) {
        context.GetResource<Rmlui::Resource::UIContext>().ProcessText(codepoint);
    });

    callbackIds.mouseButtonCallbackId =
        input.RegisterMouseButtonCallback([](Engine::Core &context, int button, int action, int mods) {
            context.GetResource<Rmlui::Resource::UIContext>().ProcessMouseButton(button, action, mods);
        });

    callbackIds.cursorPosCallbackId =
        input.RegisterCursorPosCallback([](Engine::Core &context, double xpos, double ypos) {
            context.GetResource<Rmlui::Resource::UIContext>().ProcessMouseMove(xpos, ypos, 0);
        });

    callbackIds.scrollCallbackId =
        input.RegisterScrollCallback([](Engine::Core &context, double xoffset, double yoffset) {
            context.GetResource<Rmlui::Resource::UIContext>().ProcessMouseWheel(xoffset, yoffset, 0);
        });

    uiContext.SetInputCallbackIds(callbackIds);
}
