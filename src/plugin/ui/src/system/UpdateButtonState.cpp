#include "UpdateButtonState.hpp"

#include "BoxCollider2D.hpp"
#include "CollisionUtils2D.hpp"
#include "HasChanged.hpp"
#include "InputManager.hpp"
#include "Math.hpp"
#include "Transform.hpp"

#include "Button.hpp"

void ES::Plugin::UI::System::UpdateButtonState(ES::Engine::Registry &r)
{
    auto &inputManager = r.GetResource<ES::Plugin::Input::Resource::InputManager>();
    const bool &isMouseLeftPressed = inputManager.IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT);
    const glm::vec2 &mousePos = inputManager.GetMousePosition();
    auto view = r.GetRegistry()
                    .view<ES::Plugin::UI::Component::Button, ES::Plugin::Physics::Component::BoxCollider2D,
                          ES::Plugin::Object::Component::Transform>();
    for (auto entity : view)
    {
        auto &button = view.get<ES::Plugin::UI::Component::Button>(entity);
        auto const &collider = view.get<ES::Plugin::Physics::Component::BoxCollider2D>(entity);
        auto const &transform = view.get<ES::Plugin::Object::Component::Transform>(entity);
        const ES::Plugin::Math::Rect &rect = {
            {transform.position.x,                     transform.position.y                    },
            {collider.size.x * transform.getScale().x, collider.size.y * transform.getScale().y}
        };
        button.lastState = button.state;
        if (ES::Plugin::Physics::Utils::Point2DCollidesRect2D(rect, mousePos))
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
                r))
        {
            ES::Engine::Entity(entity)
                .AddTemporaryComponent<ES::Plugin::Tools::HasChanged<ES::Plugin::UI::Component::Button>>(r);
        }
    }
}
