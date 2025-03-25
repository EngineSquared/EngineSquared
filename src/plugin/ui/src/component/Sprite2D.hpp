#pragma once

#include "Color.hpp"
#include "Math.hpp"
#include <entt/entt.hpp>
#include <glm/vec2.hpp>

namespace ES::Plugin::UI::Component {
/**
 * @brief A struct representing a 2D sprite
 */
struct Sprite2D {
    /**
     * @brief The ID of the texture to use
     */
    entt::hashed_string textureID;
    /**
     * @brief The source rectangle of the texture
     *
     * The source rectangle is the part of the texture to use.
     * The position is the upper left corner of the rectangle.
     * The size is the width and height of the rectangle.
     */
    ES::Plugin::Math::Rect sourceRect = {
        {0, 0},
        {0, 0}
    };
    /**
     * @brief Origin of the sprite, used like a pivot
     */
    glm::vec2 origin = {0, 0};
    /**
     * @brief The depth of the sprite
     */
    int zIndex = 0;
    /**
     * @brief The color of the sprite
     */
    ES::Plugin::Colors::Utils::Color color;
};
} // namespace ES::Plugin::UI::Component
