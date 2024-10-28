#pragma once

#include <glm/glm.hpp>

namespace ES::Plugin::Physics::Component {
/**
 * Component used to represent a soft body node in a soft body simulation.
 * A soft body node is a point in a soft body that can be connected to other nodes.
 */
struct SoftBodyNode {
    /**
     * Mass of the node.
     */
    float mass;
    /**
     * Inverse mass of the node.
     */
    float inverseMass;
    /**
     * Damping factor applied to the node force.
     */
    float damping;
    /**
     * Fricion coefficient of the node.
     */
    float friction;
    /**
     * Elasticity of the node.
     */
    float elasticity;
    /**
     * Velocity of the node.
     */
    glm::vec3 velocity;
    /**
     * Force applied to the node.
     */
    glm::vec3 force;

    SoftBodyNode(float mass = 1, float damping = 0.95f, float friction = 0.5f, float elasticity = 0.9f)
        : mass(mass), inverseMass(mass >= 0 ? 1 / mass : 0), damping(damping), friction(friction),
          elasticity(elasticity), velocity(0), force(0)
    {
    }

    /**
     * Apply a force to the node.
     * @param force Force to apply.
     */
    void ApplyForce(glm::vec3 force) { this->force += force; }
};
} // namespace ES::Plugin::Physics::Component
