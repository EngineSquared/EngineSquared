#pragma once

#include "Math.hpp"

#include "BoxCollider3D.hpp"

namespace ES::Plugin::Physics::Utils {
/**
 * @brief Check if two 3D boxes are colliding
 *
 * @param   posA   The position of the first box
 * @param   boxA   The size of the first box
 * @param   posB   The position of the second box
 * @param   boxB   The size of the second box
 */
bool Box3DCollidesBox3D(const glm::vec3 &posA, const ES::Plugin::Physics::Component::BoxCollider3D &boxA,
                        const glm::vec3 &posB, const ES::Plugin::Physics::Component::BoxCollider3D &boxB);
} // namespace ES::Plugin::Physics::Utils
