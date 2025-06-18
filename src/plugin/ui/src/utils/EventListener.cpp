#include "EventListener.hpp"

void ES::Plugin::UI::Utils::EventListener::SetCallback()
{
    auto &inputManager = _core.GetResource<ES::Plugin::Input::Resource::InputManager>();
    inputManager.RegisterMouseButtonCallback([this](const ES::Engine::Core &, int key, int action, int mods) {
        ProcessMouseButton(key, action, mods);
    });
}

void ES::Plugin::UI::Utils::EventListener::ProcessMouseButton(int button, int action, int mods)
{
    if (_context) {
        switch (action) {
            case GLFW_PRESS:
                _context->ProcessMouseButtonDown(button, ConvertKeyModifiers(mods));
                break;
            case GLFW_RELEASE:
                _context->ProcessMouseButtonUp(button, ConvertKeyModifiers(mods));
                break;
        }
    }
}

void ES::Plugin::UI::Utils::EventListener::ProcessEvent(Rml::Event &event)
{
    if (_event_callback) {
        const std::string tag = event.GetCurrentElement()->GetId();
        _event_callback(event.GetType(), tag);
    }
}

void ES::Plugin::UI::Utils::EventListener::AttachEvents(const std::string &eventType, Rml::Element &toElement)
{
    toElement.AddEventListener(eventType, this);
}

void ES::Plugin::UI::Utils::EventListener::SetEventCallback(EventCallback callback)
{
    _event_callback = std::move(callback);
}
