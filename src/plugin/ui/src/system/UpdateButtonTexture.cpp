#include "UpdateButtonTexture.hpp"
#include "OpenGL.hpp"
#include <variant>

#include "Tools.hpp"

#include "Button.hpp"
#include "Sprite2D.hpp"

static void UpdateButtonTextureColor(ES::Plugin::UI::Component::Button &button,
                                     ES::Plugin::OpenGL::Component::Sprite &sprite)
{
    auto const &displayType = std::get<ES::Plugin::UI::Component::DisplayType::TintColor>(button.displayType);
    switch (button.state)
    {
        using enum ES::Plugin::UI::Component::Button::State;
    case Normal: sprite.color = displayType.normalColor; break;
    case Hover: sprite.color = displayType.hoverColor; break;
    case Pressed: sprite.color = displayType.pressedColor; break;
    }
}

// TODO: Implement the UpdateButtonTextureImage function to update the sprite texture based on the button state.
// This function is not implemented yet because we don't currently have textures.
static void UpdateButtonTextureImage(ES::Plugin::UI::Component::Button &, ES::Plugin::OpenGL::Component::Sprite &)
{
    // auto const &displayType = std::get<ES::Plugin::UI::Component::DisplayType::Image>(button.displayType);
    // switch (button.state)
    // {
    //     using enum ES::Plugin::UI::Component::Button::State;
    // case Normal: sprite.textureID = displayType.normalImageID; break;
    // case Hover: sprite.textureID = displayType.hoverImageID; break;
    // case Pressed: sprite.textureID = displayType.pressedImageID; break;
    // }
}

void ES::Plugin::UI::System::UpdateButtonTexture(ES::Engine::Core &core)
{
    auto view = core.GetRegistry()
                    .view<ES::Plugin::UI::Component::Button, ES::Plugin::OpenGL::Component::Sprite,
                          ES::Plugin::Tools::HasChanged<ES::Plugin::UI::Component::Button>>();
    for (auto entity : view)
    {
        auto &button = view.get<ES::Plugin::UI::Component::Button>(entity);
        auto &sprite = view.get<ES::Plugin::OpenGL::Component::Sprite>(entity);
        if (std::holds_alternative<ES::Plugin::UI::Component::DisplayType::TintColor>(button.displayType))
        {
            UpdateButtonTextureColor(button, sprite);
        }
        else if (std::holds_alternative<ES::Plugin::UI::Component::DisplayType::Image>(button.displayType))
        {
            UpdateButtonTextureImage(button, sprite);
        }
    }
}
