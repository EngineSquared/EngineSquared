#include "CollisionUtils2D.hpp"

bool ES::Plugin::Physics::Utils::Point2DCollidesRect2D(const ES::Plugin::Math::Rect &rect, const glm::vec2 &point)
{
    return point.x >= rect.position.x && point.x <= rect.position.x + rect.size.x && point.y >= rect.position.y &&
           point.y <= rect.position.y + rect.size.y;
}
