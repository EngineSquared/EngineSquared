#include <glm/gtc/quaternion.hpp>

#include "Math.hpp"

glm::vec3 ES::Plugin::Math::QuaternionToEuler(const glm::quat &q) { return glm::eulerAngles(q); }
