#pragma once

#include "Math.hpp"

namespace ES::Plugin::Collision::Utils {
/**
 * @brief Check if a point is inside a rectangle
 *
 * @param   rect    The rectangle to use
 * @param   point   The point to check
 * @return  true    If the point is inside the rectangle
 * @return  false   If the point is outside the rectangle
 */
bool CollidePointRect(const ES::Plugin::Math::Rect &rect, const glm::vec2 &point);
} // namespace ES::Plugin::Collision::Utils
