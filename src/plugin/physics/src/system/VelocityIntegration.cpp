#include "VelocityIntegration.hpp"
#include "ParticleBoxCollision.hpp"
#include "RelativeTimeUpdate.hpp"
#include "SoftBodyNode.hpp"
#include "SoftBodySpring.hpp"
#include "Transform.hpp"

namespace ES::Plugin::Physics::System {
constexpr float GRAVITY = 9.81f;

static void ApplyGravity(ES::Engine::Core &core)
{
    auto nodeView = core.GetRegistry().view<ES::Plugin::Physics::Component::SoftBodyNode>();

    for (auto entity : nodeView)
    {
        auto &node = nodeView.get<ES::Plugin::Physics::Component::SoftBodyNode>(entity);
        node.ApplyForce(glm::vec3(0, -GRAVITY, 0));
    }
}

static void ApplySpringForces(ES::Engine::Core &core)
{
    auto springView = core.GetRegistry().view<ES::Plugin::Physics::Component::SoftBodySpring>();

    for (auto entity : springView)
    {
        auto &spring = springView.get<ES::Plugin::Physics::Component::SoftBodySpring>(entity);

        spring.ApplyForce(core);
    }
}

static void IntegrateVelocities(ES::Engine::Core &core)
{
    auto nodeView = core.GetRegistry()
                        .view<ES::Plugin::Physics::Component::SoftBodyNode, ES::Plugin::Object::Component::Transform>();

    float dt = core.GetScheduler<ES::Engine::Scheduler::RelativeTimeUpdate>().GetCurrentDeltaTime();

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

void VelocityIntegration(ES::Engine::Core &core)
{
    ApplyGravity(core);
    ApplySpringForces(core);
    IntegrateVelocities(core);
}
} // namespace ES::Plugin::Physics::System
