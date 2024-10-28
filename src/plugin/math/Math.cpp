#include <glm/gtc/quaternion.hpp>

#include "math.hpp"

float ES::Plugin::Math::QuaternionToAngle(const glm::quat &q)
{
    float x = q.x;
    float y = q.y;

    float angle = 2.0f * atan2(y, x);
    return glm::degrees(angle);
}
