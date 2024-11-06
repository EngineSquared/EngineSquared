#pragma once

#include <functional>
#include <variant>

#include <entt/entt.hpp>

#include "Engine.hpp"

#include "AssetID.hpp"
#include "Color.hpp"

namespace ES::Plugin::UI::Component {
/**
 * @brief The different types of display for a button
 */
namespace DisplayType {
/**
 * @brief A struct representing the tint color of a button
 *
 * @param   imageID         The ID of the image to tint
 * @param   normalColor     The color to use when the button is in the normal state
 * @param   hoverColor      The color to use when the button is in the hover state
 * @param   pressedColor    The color to use when the button is in the pressed state
 */
struct TintColor {
    ES::Plugin::Object::Utils::AssetID imageID;
    ES::Plugin::Colors::Utils::Color normalColor;
    ES::Plugin::Colors::Utils::Color hoverColor;
    ES::Plugin::Colors::Utils::Color pressedColor;
};

/**
 * @brief A struct representing the image of a button
 *
 * @param   normalImageID   The ID of the image to use when the button is in the normal state
 * @param   hoverImageID    The ID of the image to use when the button is in the hover state
 * @param   pressedImageID  The ID of the image to use when the button is in the pressed state
 */
struct Image {
    ES::Plugin::Object::Utils::AssetID normalImageID;
    ES::Plugin::Object::Utils::AssetID hoverImageID;
    ES::Plugin::Object::Utils::AssetID pressedImageID;
};

/**
 * @brief A variant representing the different types of display for a button
 */
using Variant = std::variant<TintColor, Image>;
}; // namespace DisplayType

/**
 * @brief A struct representing a button
 */
struct Button {
    /**
     * @brief The state of the button
     */
    enum class State {
        Normal,
        Hover,
        Pressed
    };
    /**
     * @brief The current state of the button
     */
    State state = State::Normal;
    /**
     * @brief The previous state of the button, used to check if the state has changed
     */
    State lastState = State::Normal;
    /**
     * @brief The display type of the button
     */
    DisplayType::Variant displayType;
    /**
     * @brief The function to call when the button is clicked
     */
    std::function<void(ES::Engine::Registry &)> onClick;
};
}; // namespace ES::Plugin::UI::Component
