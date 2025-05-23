#include "UI.pch.hpp"

#include "OpenGL.hpp"
#include "UpdateButtonTexture.hpp"

#include "Tools.hpp"

#include "Button.hpp"
#include "Sprite.hpp"

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

static void UpdateButtonTextureImage(const ES::Plugin::UI::Component::Button &button,
                                     ES::Plugin::OpenGL::Component::TextureHandle &textureHandle)
{
    auto const &displayType = std::get<ES::Plugin::UI::Component::DisplayType::Image>(button.displayType);
    switch (button.state)
    {
        using enum ES::Plugin::UI::Component::Button::State;
    case Normal: textureHandle = displayType.normal; break;
    case Hover: textureHandle = displayType.hover; break;
    case Pressed: textureHandle = displayType.pressed; break;
    }
}

void ES::Plugin::UI::System::UpdateButtonTexture(ES::Engine::Core &core)
{
    auto view = core.GetRegistry()
                    .view<ES::Plugin::UI::Component::Button, ES::Plugin::OpenGL::Component::Sprite,
                          ES::Utils::Tools::HasChanged<ES::Plugin::UI::Component::Button>>();
    view.each(
        [&core](auto e, ES::Plugin::UI::Component::Button &button, ES::Plugin::OpenGL::Component::Sprite &sprite) {
            if (std::holds_alternative<ES::Plugin::UI::Component::DisplayType::TintColor>(button.displayType))
            {
                UpdateButtonTextureColor(button, sprite);
            }
            else if (std::holds_alternative<ES::Plugin::UI::Component::DisplayType::Image>(button.displayType))
            {
                ES::Engine::Entity entity(e);
                ES::Plugin::OpenGL::Component::TextureHandle *textureHandle =
                    entity.TryGetComponent<ES::Plugin::OpenGL::Component::TextureHandle>(core);
                if (!textureHandle)
                {
                    ES::Utils::Log::Warn(fmt::format("Button {} has no texture handle", (unsigned int) (entity)));
                    return;
                }
                UpdateButtonTextureImage(button, *textureHandle);
            }
        });
}
