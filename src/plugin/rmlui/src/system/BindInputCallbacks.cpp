#include "system/BindInputCallbacks.hpp"

#include "Logger.hpp"
#include "core/Core.hpp"
#include "resource/InputManager.hpp"
#include "resource/UIContext.hpp"

void Rmlui::System::BindInputCallbacks(Engine::Core &core)
{
    static bool registered = false;
    if (registered)
    {
        Log::Warn("Rmlui input callbacks already registered");
        return;
    }
    registered = true;

    auto &input = core.GetResource<Input::Resource::InputManager>();

    input.RegisterKeyCallback([](Engine::Core &context, int key, int scancode, int action, int mods) {
        (void) scancode; // Might not be useful?
        context.GetResource<Rmlui::Resource::UIContext>().ProcessKey(key, action, mods);
    });

    input.RegisterCharCallback([](Engine::Core &context, unsigned int codepoint) {
        context.GetResource<Rmlui::Resource::UIContext>().ProcessText(codepoint);
    });

    input.RegisterMouseButtonCallback([](Engine::Core &context, int button, int action, int mods) {
        context.GetResource<Rmlui::Resource::UIContext>().ProcessMouseButton(button, action, mods);
    });

    input.RegisterCursorPosCallback([](Engine::Core &context, double xpos, double ypos) {
        context.GetResource<Rmlui::Resource::UIContext>().ProcessMouseMove(xpos, ypos, 0);
    });

    input.RegisterScrollCallback([](Engine::Core &context, double xoffset, double yoffset) {
        context.GetResource<Rmlui::Resource::UIContext>().ProcessMouseWheel(xoffset, yoffset, 0);
    });
}
