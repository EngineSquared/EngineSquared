#pragma once

#include "Math.hpp"

namespace ES::Plugin::Collision::Utils {
bool CollidePointRect(const ES::Plugin::Math::Rect &rect, const glm::vec2 &point);
}
