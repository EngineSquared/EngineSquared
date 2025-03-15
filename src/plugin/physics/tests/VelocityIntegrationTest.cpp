#include <gtest/gtest.h>

#include <chrono>
#include <thread>

#include "Core.hpp"
#include "Entity.hpp"
#include "SoftBodyNode.hpp"
#include "Transform.hpp"
#include "VelocityIntegration.hpp"

#include "utils/Sleep.hpp"

TEST(VelocityIntegration, BasicGravityIntegration)
{
    ES::Engine::Core core;
    core.RegisterSystem<ES::Engine::Scheduler::RelativeTimeUpdate>(ES::Plugin::Physics::System::VelocityIntegration);

    ES::Engine::Entity entity = core.CreateEntity();
    core.GetRegistry().emplace<ES::Plugin::Object::Component::Transform>(entity, glm::vec3(0));
    core.GetRegistry().emplace<ES::Plugin::Physics::Component::SoftBodyNode>(entity);

    auto const &node = core.GetRegistry().get<ES::Plugin::Physics::Component::SoftBodyNode>(entity);
    auto const &transform = core.GetRegistry().get<ES::Plugin::Object::Component::Transform>(entity);

    // registry uses a real time provider to get the elapsed time, so we should sleep a bit
    SleepFor(10);
    core.RunSystems();
    // gravity is applied to the node, so the position should be negative
    EXPECT_LT(transform.position.y, 0);
    // velocity should be as well
    EXPECT_LT(node.velocity.y, 0);
    // force should be zero though
    EXPECT_EQ(node.force.y, 0);
}

TEST(VelocityIntegration, ForceHigherThanGravity)
{
    ES::Engine::Core core;
    core.RegisterSystem<ES::Engine::Scheduler::RelativeTimeUpdate>(ES::Plugin::Physics::System::VelocityIntegration);

    ES::Engine::Entity entity = core.CreateEntity();
    core.GetRegistry().emplace<ES::Plugin::Object::Component::Transform>(entity, glm::vec3(0));
    core.GetRegistry().emplace<ES::Plugin::Physics::Component::SoftBodyNode>(entity);

    auto &node = core.GetRegistry().get<ES::Plugin::Physics::Component::SoftBodyNode>(entity);
    auto const &transform = core.GetRegistry().get<ES::Plugin::Object::Component::Transform>(entity);

    node.ApplyForce(glm::vec3(0, 100, 0));

    SleepFor(10);
    core.RunSystems();
    // here position should be positive
    EXPECT_GT(transform.position.y, 0);
    // velocity should be as well
    EXPECT_GT(node.velocity.y, 0);
    // force should be zero though
    EXPECT_EQ(node.force.y, 0);
}
