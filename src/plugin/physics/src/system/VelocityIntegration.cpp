#include "VelocityIntegration.hpp"
#include "ParticleBoxCollision.hpp"
#include "RealTimeProvider.hpp"
#include "SoftBodyNode.hpp"
#include "SoftBodySpring.hpp"
#include "Transform.hpp"

namespace ES::Plugin::Physics::System {
constexpr float GRAVITY = 9.81f;

static void ApplyGravity(ES::Engine::Registry &registry)
{
    auto nodeView = registry.GetRegistry()
                        .view<ES::Plugin::Physics::Component::SoftBodyNode, ES::Plugin::Object::Component::Transform>();

    for (auto entity : nodeView)
    {
        auto &node = nodeView.get<ES::Plugin::Physics::Component::SoftBodyNode>(entity);
        auto &transform = nodeView.get<ES::Plugin::Object::Component::Transform>(entity);

        node.ApplyForce(glm::vec3(0, -GRAVITY, 0));
    }
}

static void ApplySpringForces(ES::Engine::Registry &registry)
{
    auto springView = registry.GetRegistry().view<ES::Plugin::Physics::Component::SoftBodySpring>();

    for (auto entity : springView)
    {
        auto &spring = springView.get<ES::Plugin::Physics::Component::SoftBodySpring>(entity);

        spring.ApplyForce(registry);
    }
}

static void IntegrateVelocities(ES::Engine::Registry &registry)
{
    auto nodeView = registry.GetRegistry()
                        .view<ES::Plugin::Physics::Component::SoftBodyNode, ES::Plugin::Object::Component::Transform>();

    float dt = registry.GetFixedDeltaTime();

    for (auto entity : nodeView)
    {
        auto &node = nodeView.get<ES::Plugin::Physics::Component::SoftBodyNode>(entity);
        auto &transform = nodeView.get<ES::Plugin::Object::Component::Transform>(entity);

        if (node.mass == 0)
        {
            continue;
        }

        glm::vec3 acceleration = node.force * node.inverseMass;
        node.velocity += acceleration * dt;
        transform.position += node.velocity * dt;
        node.force = glm::vec3(0);
        node.velocity *= node.damping;
    }
}

void VelocityIntegration(ES::Engine::Registry &registry)
{
    ApplyGravity(registry);
    ApplySpringForces(registry);
    IntegrateVelocities(registry);
}
} // namespace ES::Plugin::Physics::System
