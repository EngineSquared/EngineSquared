#include <gtest/gtest.h>

#include <chrono>
#include <thread>

#include "Entity.hpp"
#include "RealTimeProvider.hpp"
#include "RealTimeUpdater.hpp"
#include "Registry.hpp"
#include "SoftBodyNode.hpp"
#include "Transform.hpp"
#include "VelocityIntegration.hpp"

#include "utils/Sleep.hpp"

TEST(VelocityIntegration, BasicGravityIntegration)
{
    ES::Engine::Registry registry;
    registry.RegisterResource<ES::Plugin::Time::Resource::RealTimeProvider>(
        ES::Plugin::Time::Resource::RealTimeProvider());
    registry.RegisterSystem(ES::Plugin::Time::System::RealTimeUpdater);
    registry.RegisterSystem(ES::Plugin::Physics::System::VelocityIntegration);

    ES::Engine::Entity entity = registry.CreateEntity();
    registry.GetRegistry().emplace<ES::Plugin::Object::Component::Transform>(entity, glm::vec3(0));
    registry.GetRegistry().emplace<ES::Plugin::Physics::Component::SoftBodyNode>(entity);

    auto const &node = registry.GetRegistry().get<ES::Plugin::Physics::Component::SoftBodyNode>(entity);
    auto const &transform = registry.GetRegistry().get<ES::Plugin::Object::Component::Transform>(entity);

    // registry uses a real time provider to get the elapsed time, so we should sleep a bit
    SleepFor(10);
    registry.RunSystems();
    // gravity is applied to the node, so the position should be negative
    EXPECT_LT(transform.position.y, 0);
    // velocity should be as well
    EXPECT_LT(node.velocity.y, 0);
    // force should be zero though
    EXPECT_EQ(node.force.y, 0);
}

TEST(VelocityIntegration, ForceHigherThanGravity)
{
    ES::Engine::Registry registry;
    registry.RegisterResource<ES::Plugin::Time::Resource::RealTimeProvider>(
        ES::Plugin::Time::Resource::RealTimeProvider());
    registry.RegisterSystem(ES::Plugin::Time::System::RealTimeUpdater);
    registry.RegisterSystem(ES::Plugin::Physics::System::VelocityIntegration);

    ES::Engine::Entity entity = registry.CreateEntity();
    registry.GetRegistry().emplace<ES::Plugin::Object::Component::Transform>(entity, glm::vec3(0));
    registry.GetRegistry().emplace<ES::Plugin::Physics::Component::SoftBodyNode>(entity);

    auto &node = registry.GetRegistry().get<ES::Plugin::Physics::Component::SoftBodyNode>(entity);
    auto const &transform = registry.GetRegistry().get<ES::Plugin::Object::Component::Transform>(entity);

    node.ApplyForce(glm::vec3(0, 100, 0));

    SleepFor(10);
    registry.RunSystems();
    // here position should be positive
    EXPECT_GT(transform.position.y, 0);
    // velocity should be as well
    EXPECT_GT(node.velocity.y, 0);
    // force should be zero though
    EXPECT_EQ(node.force.y, 0);
}
