#include <glm/glm.hpp>

namespace ES::Plugin::Collision::Component {

/**
 * Basic collider used to check collisions through CollisionChecker system
 */
struct BoxCollider3D {
    /**
     * The size of the box
     */
    glm::vec3 size;
};
} // namespace ES::Plugin::Collision::Component
