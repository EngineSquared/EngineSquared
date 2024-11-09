#include "UpdateButtonTexture.hpp"

#include <variant>

#include "Tools.hpp"

#include "Button.hpp"
#include "Sprite2D.hpp"

static void UpdateButtonTextureColor(const entt::entity &entity, ES::Plugin::UI::Component::Button &button,
                                     ES::Plugin::UI::Component::Sprite2D &sprite)
{
    auto &displayType = std::get<ES::Plugin::UI::Component::DisplayType::TintColor>(button.displayType);
    switch (button.state)
    {
    case ES::Plugin::UI::Component::Button::State::Normal: sprite.color = displayType.normalColor; break;
    case ES::Plugin::UI::Component::Button::State::Hover: sprite.color = displayType.hoverColor; break;
    case ES::Plugin::UI::Component::Button::State::Pressed: sprite.color = displayType.pressedColor; break;
    }
}

static void UpdateButtonTextureImage(const entt::entity &entity, ES::Plugin::UI::Component::Button &button,
                                     ES::Plugin::UI::Component::Sprite2D &sprite)
{
    auto &displayType = std::get<ES::Plugin::UI::Component::DisplayType::Image>(button.displayType);
    switch (button.state)
    {
    case ES::Plugin::UI::Component::Button::State::Normal: sprite.textureID = displayType.normalImageID; break;
    case ES::Plugin::UI::Component::Button::State::Hover: sprite.textureID = displayType.hoverImageID; break;
    case ES::Plugin::UI::Component::Button::State::Pressed: sprite.textureID = displayType.pressedImageID; break;
    }
}

void ES::Plugin::UI::System::UpdateButtonTexture(ES::Engine::Registry &r)
{
    auto view = r.GetRegistry()
                    .view<ES::Plugin::UI::Component::Button, ES::Plugin::UI::Component::Sprite2D,
                          ES::Plugin::Tools::HasChanged<ES::Plugin::UI::Component::Button>>();
    for (auto entity : view)
    {
        auto &button = view.get<ES::Plugin::UI::Component::Button>(entity);
        auto &sprite = view.get<ES::Plugin::UI::Component::Sprite2D>(entity);
        if (std::holds_alternative<ES::Plugin::UI::Component::DisplayType::TintColor>(button.displayType))
        {
            UpdateButtonTextureColor(entity, button, sprite);
        }
        else if (std::holds_alternative<ES::Plugin::UI::Component::DisplayType::Image>(button.displayType))
        {
            UpdateButtonTextureImage(entity, button, sprite);
        }
    }
}
