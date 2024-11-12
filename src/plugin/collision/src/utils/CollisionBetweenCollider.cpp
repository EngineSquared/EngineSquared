#include "CollisionBetweenCollider.hpp"

bool ES::Plugin::Collision::Utils::CollidePointRect(const ES::Plugin::Math::Rect &rect, const glm::vec2 &point)
{
    return point.x >= rect.position.x && point.x <= rect.position.x + rect.size.x && point.y >= rect.position.y &&
           point.y <= rect.position.y + rect.size.y;
}
