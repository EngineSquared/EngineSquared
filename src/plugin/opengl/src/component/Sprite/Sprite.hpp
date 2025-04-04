#pragma once

#include <string>

#include <entt/entt.hpp>
#include "Colors.hpp"
#include "Math.hpp"

namespace ES::Plugin::OpenGL::Component {
struct Sprite {
    ES::Plugin::Colors::Utils::Color color = ES::Plugin::Colors::Utils::MAGENTA_COLOR;
    ES::Plugin::Math::Rect rect = {{0.f, 0.f}, {32.f, 32.f}};
};
} // namespace ES::Plugin::OpenGL::Component
