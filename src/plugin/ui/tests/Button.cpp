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
    r.RegisterSystem(System::ButtonClick);
    r.RegisterSystem(ES::Engine::Entity::RemoveTemporaryComponents);

    r.RegisterResource<onClickCalled>(onClickCalled());

    auto button = ES::Engine::Entity(r.CreateEntity());
    button.AddComponent<Component::Button>(r);

    auto &buttonComponent = button.GetComponents<Component::Button>(r);
    buttonComponent.lastState = Component::Button::State::Pressed;
    buttonComponent.state = Component::Button::State::Hover;

    buttonComponent.onClick = [](ES::Engine::Registry &r) { r.GetResource<onClickCalled>().clicked = true; };

    EXPECT_FALSE(r.GetResource<onClickCalled>().clicked);

    button.AddTemporaryComponent<ES::Plugin::Tools::HasChanged<Component::Button>>(r);
    r.RunSystems();

    EXPECT_TRUE(r.GetResource<onClickCalled>().clicked);
}

TEST(Button, UpdateButtonTexture)
{
    ES::Engine::Registry r;

    r.RegisterSystem(System::UpdateButtonTexture);
    r.RegisterSystem(ES::Engine::Entity::RemoveTemporaryComponents);

    auto button = ES::Engine::Entity(r.CreateEntity());
    button.AddComponent<Component::Button>(r);
    button.AddComponent<Component::Sprite2D>(r);

    auto &buttonComponent = button.GetComponents<Component::Button>(r);
    buttonComponent.displayType =
        Component::DisplayType::TintColor{.imageID = ES::Plugin::Object::Utils::NULL_ASSET_ID,
                                          .normalColor = ES::Plugin::Colors::Utils::WHITE_COLOR,
                                          .hoverColor = ES::Plugin::Colors::Utils::GRAY_COLOR,
                                          .pressedColor = ES::Plugin::Colors::Utils::DARKGRAY_COLOR};

    buttonComponent.state = Component::Button::State::Hover;
    button.AddTemporaryComponent<ES::Plugin::Tools::HasChanged<Component::Button>>(r);
    r.RunSystems();
    EXPECT_EQ(button.GetComponents<Component::Sprite2D>(r).color, ES::Plugin::Colors::Utils::GRAY_COLOR);

    buttonComponent.state = Component::Button::State::Pressed;
    button.AddTemporaryComponent<ES::Plugin::Tools::HasChanged<Component::Button>>(r);
    r.RunSystems();
    EXPECT_EQ(button.GetComponents<Component::Sprite2D>(r).color, ES::Plugin::Colors::Utils::DARKGRAY_COLOR);

    buttonComponent.state = Component::Button::State::Normal;
    button.AddTemporaryComponent<ES::Plugin::Tools::HasChanged<Component::Button>>(r);
    r.RunSystems();
    EXPECT_EQ(button.GetComponents<Component::Sprite2D>(r).color, ES::Plugin::Colors::Utils::WHITE_COLOR);
}
