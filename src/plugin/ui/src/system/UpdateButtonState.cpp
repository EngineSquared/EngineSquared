#include "UpdateButtonState.hpp"

#include "BoxCollider2D.hpp"
#include "CollisionUtils2D.hpp"
#include "HasChanged.hpp"
#include "InputUtils.hpp"
#include "Math.hpp"
#include "Transform.hpp"
#include "Window.hpp"

#include "Button.hpp"

void ES::Plugin::UI::System::UpdateButtonState(ES::Engine::Core &core)
{
    auto &window = core.GetResource<ES::Plugin::Window::Resource::Window>();
    const bool &isMouseLeftPressed = Input::Utils::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT);
    glm::vec2 mousePos = window.GetMousePosition();

    auto view = core.GetRegistry()
                    .view<ES::Plugin::UI::Component::Button, ES::Plugin::UI::Component::BoxCollider2D,
                          ES::Plugin::Object::Component::Transform>();
    for (auto entity : view)
    {
        auto &button = view.get<ES::Plugin::UI::Component::Button>(entity);
        auto const &collider = view.get<ES::Plugin::UI::Component::BoxCollider2D>(entity);
        auto const &transform = view.get<ES::Plugin::Object::Component::Transform>(entity);
        const ES::Plugin::Math::Rect &rect = {
            {transform.position.x,                     transform.position.y                    },
            {collider.size.x * transform.getScale().x, collider.size.y * transform.getScale().y}
        };
        button.lastState = button.state;
        if (ES::Plugin::UI::Utils::Point2DCollidesRect2D(rect, mousePos))
        {
            if (isMouseLeftPressed)
            {
                button.state = ES::Plugin::UI::Component::Button::State::Pressed;
            }
            else
            {
                button.state = ES::Plugin::UI::Component::Button::State::Hover;
            }
        }
        else
        {
            button.state = ES::Plugin::UI::Component::Button::State::Normal;
        }
        if (button.lastState != button.state &&
            !ES::Engine::Entity(entity).HasComponents<ES::Plugin::Tools::HasChanged<ES::Plugin::UI::Component::Button>>(
                core))
        {
            ES::Engine::Entity(entity)
                .AddTemporaryComponent<ES::Plugin::Tools::HasChanged<ES::Plugin::UI::Component::Button>>(core);
        }
    }
}
