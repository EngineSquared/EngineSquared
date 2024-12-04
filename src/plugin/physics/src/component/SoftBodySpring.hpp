#pragma once

#include <glm/glm.hpp>

#include "Engine.hpp"
#include "SoftBodyNode.hpp"
#include "Transform.hpp"

namespace ES::Plugin::Physics::Component {
/**
 * Component used to represent a spring in a soft body simulation.
 * A spring is a connection between two nodes in a soft body.
 */
struct SoftBodySpring {
    /**
     * Stiffness of the spring.
     */
    float stiffness;
    /**
     * Damping factor applied to the spring force.
     */
    float damping;
    /**
     * Rest length of the spring.
     */
    float restLength;
    /**
     * Entity of the first node connected to the spring.
     * This entity should have a SoftBodyNode component.
     */
    ES::Engine::Entity entityNodeA;
    /**
     * Entity of the second node connected to the spring.
     * This entity should have a SoftBodyNode component.
     */
    ES::Engine::Entity entityNodeB;

    SoftBodySpring(ES::Engine::Entity nodeA, ES::Engine::Entity nodeB, float stiffness = 1, float damping = 0.99f,
                   float restLength = 1)
        : stiffness(stiffness), damping(damping), restLength(restLength), entityNodeA(nodeA), entityNodeB(nodeB)
    {
    }

    /**
     * Apply a force to the spring.
     */
    void ApplyForce(ES::Engine::Registry &registry)
    {
        auto &nodeA = registry.GetRegistry().get<ES::Plugin::Physics::Component::SoftBodyNode>(this->entityNodeA);
        auto &nodeB = registry.GetRegistry().get<ES::Plugin::Physics::Component::SoftBodyNode>(this->entityNodeB);
        auto &transformA = registry.GetRegistry().get<ES::Plugin::Object::Component::Transform>(this->entityNodeA);
        auto &transformB = registry.GetRegistry().get<ES::Plugin::Object::Component::Transform>(this->entityNodeB);

        auto direction = transformB.position - transformA.position;
        float currentLength = glm::length(direction);
        float stretch = currentLength - this->restLength;
        glm::vec3 springForce = this->stiffness * stretch * glm::normalize(direction);

        glm::vec3 relativeVelocity = nodeA.velocity - nodeB.velocity;
        glm::vec3 dampingForce =
            glm::normalize(direction) * (damping * glm::dot(relativeVelocity, glm::normalize(direction)));

        glm::vec3 totalForce = springForce - dampingForce;

        nodeA.ApplyForce(totalForce);
        nodeB.ApplyForce(-totalForce);
    }
};
} // namespace ES::Plugin::Physics::Component
