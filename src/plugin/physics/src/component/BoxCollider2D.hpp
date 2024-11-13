#pragma once

#include <glm/glm.hpp>

namespace ES::Plugin::Physics::Component {

/**
 * Basic collider used to check collisions through CollisionChecker system
 */
struct BoxCollider2D {
    /**
     * The size of the box
     */
    glm::vec2 size;
};
} // namespace ES::Plugin::Physics::Component
