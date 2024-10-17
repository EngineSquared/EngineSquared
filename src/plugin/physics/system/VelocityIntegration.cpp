#include "VelocityIntegration.hpp"
#include "SoftBodyNode.hpp"
#include "SoftBodySpring.hpp"
#include "Transform.hpp"
#include "RealTimeProvider.hpp"


namespace ES::Plugin::Physics {
constexpr float GRAVITY = 9.81f;

void System::VelocityIntegration(ES::Engine::Registry &registry)
{
    auto realTimeProvider = registry.GetResource<ES::Plugin::Time::Resource::RealTimeProvider>();
    auto nodeView = registry.GetRegistry()
                        .view<ES::Plugin::Physics::Component::SoftBodyNode, ES::Plugin::Object::Component::Transform>();

    for (auto entity : nodeView)
    {
        auto &node = nodeView.get<ES::Plugin::Physics::Component::SoftBodyNode>(entity);
        auto &transform = nodeView.get<ES::Plugin::Object::Component::Transform>(entity);

        node.ApplyForce(glm::vec3(0, -node.mass * GRAVITY, 0));
    }

    auto springView = registry.GetRegistry().view<ES::Plugin::Physics::Component::SoftBodySpring>();

    for (auto entity : springView)
    {
        auto &spring = springView.get<ES::Plugin::Physics::Component::SoftBodySpring>(entity);

        spring.ApplyForce(registry);
    }

    for (auto entity : nodeView)
    {
        auto &node = nodeView.get<ES::Plugin::Physics::Component::SoftBodyNode>(entity);
        auto &transform = nodeView.get<ES::Plugin::Object::Component::Transform>(entity);

        float dt = realTimeProvider.GetElapsedTime();
        // printf("dt: %f\n", dt);
        glm::vec3 acceleration = node.force * node.inverseMass;
        node.velocity += acceleration * dt;
        transform.position += node.velocity * dt;
        node.Integrate(dt);
    }
}
} // namespace ES::Plugin::Physics
