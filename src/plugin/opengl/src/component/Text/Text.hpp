#pragma once

#include <glm/glm.hpp>
#include <string>

#include "Colors.hpp"

namespace ES::Plugin::UI::Component {

struct Text {
    std::string text;
    glm::vec2 position;
    float scale;
    ES::Plugin::Colors::Utils::Color color;

    Text(const std::string &text = "", glm::vec2 pos = {0.0f, 0.0f}, float scale = 1.0f,
         ES::Plugin::Colors::Utils::Color color = ES::Plugin::Colors::Utils::WHITE_COLOR)
        : text(text), position(pos), scale(scale), color(color)
    {
    }
};

} // namespace ES::Plugin::UI::Component
