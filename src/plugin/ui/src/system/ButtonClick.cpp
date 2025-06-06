#include "UI.pch.hpp"

#include "ButtonClick.hpp"

#include "Button.hpp"
#include "HasChanged.hpp"

void ES::Plugin::UI::System::ButtonClick(ES::Engine::Core &core)
{
    auto view =
        core.GetRegistry()
            .view<ES::Plugin::UI::Component::Button, ES::Utils::Tools::HasChanged<ES::Plugin::UI::Component::Button>>();
    for (auto entity : view)
    {
        auto const &button = view.get<ES::Plugin::UI::Component::Button>(entity);
        if (button.lastState == ES::Plugin::UI::Component::Button::State::Pressed &&
            button.state == ES::Plugin::UI::Component::Button::State::Hover)
        {
            button.onClick(core);
        }
    }
}
