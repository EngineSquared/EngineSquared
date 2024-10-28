#include <gtest/gtest.h>

#include "HasChanged.hpp"
#include "Object.hpp"
#include "UI.hpp"

using namespace ES::Plugin::UI;

TEST(Button, ButtonClick)
{
    struct onClickCalled {
        bool clicked = false;
    };
    ES::Engine::Registry r;

    r.RegisterResource<onClickCalled>(onClickCalled());

    ES::Engine::Entity button = ES::Engine::Entity(r.CreateEntity());
    button.AddComponent<Component::Button>(r);
    button.AddComponent<ES::Plugin::Tools::HasChanged<Component::Button>>(r);

    auto &buttonComponent = button.GetComponents<Component::Button>(r);
    buttonComponent.lastState = Component::Button::State::Pressed;
    buttonComponent.state = Component::Button::State::Hover;

    buttonComponent.onClick.connect<+[](ES::Engine::Registry &r) { r.GetResource<onClickCalled>().clicked = true; }>();

    EXPECT_FALSE(r.GetResource<onClickCalled>().clicked);

    System::ButtonClick(r);

    EXPECT_TRUE(r.GetResource<onClickCalled>().clicked);
}

TEST(Button, UpdateButtonTexture)
{
    ES::Engine::Registry r;

    ES::Engine::Entity button = ES::Engine::Entity(r.CreateEntity());
    button.AddComponent<Component::Button>(r);
    button.AddComponent<ES::Plugin::Tools::HasChanged<Component::Button>>(r);
    button.AddComponent<Component::Sprite2D>(r);

    auto &buttonComponent = button.GetComponents<Component::Button>(r);
    buttonComponent.displayType =
        Component::DisplayType::TintColor{.imageID = ES::Plugin::Object::Utils::NULL_ASSET_ID,
                                          .normalColor = ES::Plugin::Colors::Utils::WHITE_COLOR,
                                          .hoverColor = ES::Plugin::Colors::Utils::GRAY_COLOR,
                                          .pressedColor = ES::Plugin::Colors::Utils::DARKGRAY_COLOR};

    buttonComponent.state = Component::Button::State::Hover;
    System::UpdateButtonTexture(r);
    EXPECT_EQ(button.GetComponents<Component::Sprite2D>(r).color, ES::Plugin::Colors::Utils::GRAY_COLOR);

    buttonComponent.state = Component::Button::State::Pressed;
    System::UpdateButtonTexture(r);
    EXPECT_EQ(button.GetComponents<Component::Sprite2D>(r).color, ES::Plugin::Colors::Utils::DARKGRAY_COLOR);

    buttonComponent.state = Component::Button::State::Normal;
    System::UpdateButtonTexture(r);
    EXPECT_EQ(button.GetComponents<Component::Sprite2D>(r).color, ES::Plugin::Colors::Utils::WHITE_COLOR);
}
