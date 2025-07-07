/*
 * The RmlUi backend is mostly taken from the official implementation of the RmlUi GLFW GL3 backend
 * See: https://github.dev/mikke89/RmlUi/blob/master/Backends/RmlUi_Renderer_GL3.h
 */

#pragma once

#include <RmlUi/Core.h>
#include <RmlUi/Core/Event.h>

#include "Core.hpp"
#include "Input.hpp"

namespace ES::Plugin::UI::Utils {
class EventListener : public Rml::EventListener {
  public:
    using EventCallback = std::function<void(const std::string &event_name, const std::string &element_id)>;

  private:
    Rml::Context *_context;
    EventCallback _event_callback;

    static int ConvertKeyModifiers(int glfw_mods)
    {
        int key_modifier_state = 0;
        if (GLFW_MOD_SHIFT & glfw_mods)
            key_modifier_state |= Rml::Input::KM_SHIFT;
        if (GLFW_MOD_CONTROL & glfw_mods)
            key_modifier_state |= Rml::Input::KM_CTRL;
        if (GLFW_MOD_ALT & glfw_mods)
            key_modifier_state |= Rml::Input::KM_ALT;
        if (GLFW_MOD_CAPS_LOCK & glfw_mods)
            key_modifier_state |= Rml::Input::KM_SCROLLLOCK;
        if (GLFW_MOD_NUM_LOCK & glfw_mods)
            key_modifier_state |= Rml::Input::KM_NUMLOCK;
        return key_modifier_state;
    }

  public:
    EventListener() = delete;
    explicit EventListener(Rml::Context &context) : _context(&context) {}

    void SetCallback(ES::Engine::Core &core);
    void ProcessMouseButton(int button, int action, int mods);
    void AttachEvents(const std::string &eventType, Rml::Element &toElement);
    void SetEventCallback(EventCallback callback);

    void ProcessEvent(Rml::Event &event) override;
};
} // namespace ES::Plugin::UI::Utils
