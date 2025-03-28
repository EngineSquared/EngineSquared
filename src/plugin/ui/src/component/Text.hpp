#pragma once

#include <glm/glm.hpp>
#include <string>

namespace ES::Plugin::UI::Component {

struct Text {
    std::string text;
    glm::vec2 position;
    float scale;
    glm::vec3 color;

    Text(const std::string &text = "", glm::vec2 pos = {0.0f, 0.0f}, float scale = 1.0f,
         glm::vec3 color = {1.0f, 1.0f, 1.0f})
        : text(text), position(pos), scale(scale), color(color)
    {
    }
};

} // namespace ES::Plugin::UI::Component
