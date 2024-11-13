#pragma once

#include "Entity.hpp"

namespace ES::Plugin::Physics::Component {

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

} // namespace ES::Plugin::Physics::Component
