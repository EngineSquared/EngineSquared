#pragma once

#include <glm/vec2.hpp>

#include "AssetID.hpp"
#include "Color.hpp"
#include "Math.hpp"

namespace ES::Plugin::UI::Component {
struct Sprite2D {
    ES::Plugin::Object::Utils::AssetID textureID = ES::Plugin::Object::Utils::NULL_ASSET_ID;
    ES::Plugin::Math::Rect sourceRect = {
        {0, 0},
        {0, 0}
    };
    glm::vec2 origin = {0, 0};
    int zIndex = 0;
    ES::Plugin::Colors::Utils::Color color;
};
} // namespace ES::Plugin::UI::Component
