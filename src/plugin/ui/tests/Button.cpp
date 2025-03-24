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
    ES::Engine::Core core;
    core.RegisterSystem(System::ButtonClick);
    core.RegisterSystem(ES::Engine::Entity::RemoveTemporaryComponents);

    core.RegisterResource<onClickCalled>(onClickCalled());

    auto button = core.CreateEntity();
    button.AddComponent<Component::Button>(core);

    auto &buttonComponent = button.GetComponents<Component::Button>(core);
    buttonComponent.lastState = Component::Button::State::Pressed;
    buttonComponent.state = Component::Button::State::Hover;

    buttonComponent.onClick = [](ES::Engine::Core &c) { c.GetResource<onClickCalled>().clicked = true; };

    EXPECT_FALSE(core.GetResource<onClickCalled>().clicked);

    button.AddTemporaryComponent<ES::Plugin::Tools::HasChanged<Component::Button>>(core);
    core.RunSystems();

    EXPECT_TRUE(core.GetResource<onClickCalled>().clicked);
}

TEST(Button, UpdateButtonTexture)
{
    ES::Engine::Core core;

    core.RegisterSystem(System::UpdateButtonTexture);
    core.RegisterSystem(ES::Engine::Entity::RemoveTemporaryComponents);

    auto button = core.CreateEntity();
    button.AddComponent<Component::Button>(core);
    button.AddComponent<Component::Sprite2D>(core);

    auto &buttonComponent = button.GetComponents<Component::Button>(core);
    buttonComponent.displayType =
        Component::DisplayType::TintColor{.imageID = ES::Plugin::Object::Utils::NULL_ASSET_ID,
                                          .normalColor = ES::Plugin::Colors::Utils::WHITE_COLOR,
                                          .hoverColor = ES::Plugin::Colors::Utils::GRAY_COLOR,
                                          .pressedColor = ES::Plugin::Colors::Utils::DARKGRAY_COLOR};

    buttonComponent.state = Component::Button::State::Hover;
    button.AddTemporaryComponent<ES::Plugin::Tools::HasChanged<Component::Button>>(core);
    core.RunSystems();
    EXPECT_EQ(button.GetComponents<Component::Sprite2D>(core).color, ES::Plugin::Colors::Utils::GRAY_COLOR);

    buttonComponent.state = Component::Button::State::Pressed;
    button.AddTemporaryComponent<ES::Plugin::Tools::HasChanged<Component::Button>>(core);
    core.RunSystems();
    EXPECT_EQ(button.GetComponents<Component::Sprite2D>(core).color, ES::Plugin::Colors::Utils::DARKGRAY_COLOR);

    buttonComponent.state = Component::Button::State::Normal;
    button.AddTemporaryComponent<ES::Plugin::Tools::HasChanged<Component::Button>>(core);
    core.RunSystems();
    EXPECT_EQ(button.GetComponents<Component::Sprite2D>(core).color, ES::Plugin::Colors::Utils::WHITE_COLOR);
}
