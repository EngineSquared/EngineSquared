#pragma once

#include "Math.hpp"

namespace ES::Plugin::Physics::Utils {
/**
 * @brief Check if a 2D point is inside a 2D rectangle
 *
 * @param   rect    The rectangle to use
 * @param   point   The point to check
 * @return  true    If the point is inside the rectangle
 * @return  false   If the point is outside the rectangle
 */
bool Point2DCollidesRect2D(const ES::Plugin::Math::Rect &rect, const glm::vec2 &point);
} // namespace ES::Plugin::Physics::Utils
