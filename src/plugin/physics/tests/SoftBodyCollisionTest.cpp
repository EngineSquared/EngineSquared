#include <gtest/gtest.h>

#include <chrono>
#include <thread>

#include "BoxCollider3D.hpp"
#include "Entity.hpp"
#include "RealTimeProvider.hpp"
#include "RealTimeUpdater.hpp"
#include "Registry.hpp"
#include "SoftBodyCollision.hpp"
#include "SoftBodyNode.hpp"
#include "Transform.hpp"
#include "VelocityIntegration.hpp"

#include "utils/Sleep.hpp"

TEST(SoftBodyCollision, BasicCollision)
{
    ES::Engine::Registry registry;

    registry.RegisterSystem(ES::Plugin::Collision::System::SoftBodyCollision);
    registry.RegisterResource<ES::Plugin::Time::Resource::RealTimeProvider>(
        ES::Plugin::Time::Resource::RealTimeProvider());
    registry.RegisterSystem(ES::Plugin::Physics::System::VelocityIntegration);
    registry.RegisterSystem(ES::Plugin::Time::System::RealTimeUpdater);

    ES::Engine::Entity particle = registry.CreateEntity();
    registry.GetRegistry().emplace<ES::Plugin::Object::Component::Transform>(particle, glm::vec3(0, 4, 0));
    registry.GetRegistry().emplace<ES::Plugin::Physics::Component::SoftBodyNode>(particle);

    ES::Engine::Entity ground = registry.CreateEntity();
    registry.GetRegistry().emplace<ES::Plugin::Object::Component::Transform>(ground, glm::vec3(0, 0, 0));
    registry.GetRegistry().emplace<ES::Plugin::Collision::Component::BoxCollider3D>(ground, glm::vec3(2, 2, 2));

    auto &node = registry.GetRegistry().get<ES::Plugin::Physics::Component::SoftBodyNode>(particle);
    auto &transform = registry.GetRegistry().get<ES::Plugin::Object::Component::Transform>(particle);

    for (int i = 0; i < 10; ++i)
    {
        SleepFor(10);
        registry.RunSystems();
    }
    // particle should have bounced and be on top of the box, so y should be positive
    EXPECT_GT(transform.position.y, 0);

    // force should be zero though
    EXPECT_EQ(node.force.y, 0);
}
