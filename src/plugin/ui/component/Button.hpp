#pragma once

#include <variant>

#include <entt/entt.hpp>

#include "AssetID.hpp"
#include "Color.hpp"

namespace ES::Plugin::UI::Component {
using AssetID = ES::Plugin::Object::Utils::AssetID;
using Color = ES::Plugin::Colors::Utils::Color;

namespace DisplayType {
struct TintColor {
    AssetID imageID;
    Color normalColor;
    Color hoverColor;
    Color pressedColor;
};

struct Image {
    AssetID normalImageID;
    AssetID hoverImageID;
    AssetID pressedImageID;
};

using Variant = std::variant<TintColor, Image>;
}; // namespace DisplayType

struct Button {
    enum class State {
        Normal,
        Hover,
        Pressed
    };
    State state = State::Normal;
    State lastState = State::Normal;
    DisplayType::Variant displayType;
    entt::delegate<void()> onClick;
};
}; // namespace ES::Plugin::UI::Component
