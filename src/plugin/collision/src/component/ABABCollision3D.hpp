#include "Entity.hpp"

#include <glm/glm.hpp>

namespace ES::Plugin::Collision::Component {

/**
 * Component that represents a collision between two entities in 3D space
 */
struct ABABCollision3D {
    /**
     * Entity 1
     */
    ES::Engine::Entity entityA;

    /**
     * Entity 2
     */
    ES::Engine::Entity entityB;
};

} // namespace ES::Plugin::Collision::Component