#pragma once

#include "Entity.hpp"

#include <glm/glm.hpp>

namespace ES::Plugin::Physics::Component {

/**
 * Component that represents a collision between a soft body particle and a box collider
 */
struct ParticleBoxCollision {
    /**
     * Entity of the particle
     */
    ES::Engine::Entity particleEntity;

    /**
     * Entity of the box collider
     */
    ES::Engine::Entity boxEntity;

    /**
     * The normal of the box collider
     * (== the normal of the face of the box collider that the particle collided with)
     */
    glm::vec3 normal;

    /**
     * The penetration depth of the particle in the box collider
     */
    float penetrationDepth;
};

} // namespace ES::Plugin::Physics::Component
