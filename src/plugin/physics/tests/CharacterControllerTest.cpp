#include <glm/glm.hpp>
#include <gtest/gtest.h>

#include "component/BoxCollider.hpp"
#include "component/CapsuleCollider.hpp"
#include "component/CharacterController.hpp"
#include "component/CharacterControllerInternal.hpp"
#include "component/RigidBody.hpp"
#include "component/Transform.hpp"
#include "core/Core.hpp"
#include "entity/Entity.hpp"
#include "plugin/PluginPhysics.hpp"
#include "resource/Time.hpp"
#include "scheduler/FixedTimeUpdate.hpp"
#include "scheduler/Shutdown.hpp"
#include "scheduler/Startup.hpp"

class TestScheduler : public Engine::Scheduler::Update {
  public:
    using Engine::Scheduler::Update::Update;
};

TEST(CharacterControllerPlugin, CharacterControllerCreation)
{
    Engine::Core core;
    core.AddPlugins<Physics::Plugin>();

    core.RegisterSystem<Engine::Scheduler::Update>(
        [&](Engine::Core &c) { c.GetResource<Engine::Resource::Time>()._elapsedTime = 0.016f; });

    core.GetScheduler<Engine::Scheduler::Startup>().RunSystems();

    auto player = core.CreateEntity();
    player.AddComponent<Object::Component::Transform>(glm::vec3(0.0f, 5.0f, 0.0f));
    player.AddComponent<Physics::Component::CapsuleCollider>(0.8f, 0.3f);
    player.AddComponent<Physics::Component::CharacterController>();

    ASSERT_TRUE(player.HasComponents<Physics::Component::CharacterControllerInternal>());

    const auto &internal = player.GetComponents<Physics::Component::CharacterControllerInternal>();
    EXPECT_TRUE(internal.IsValid());
}

TEST(CharacterControllerPlugin, CharacterFallsUnderGravity)
{
    Engine::Core c;

    c.RegisterScheduler<TestScheduler>();
    c.SetSchedulerAfter<TestScheduler, Engine::Scheduler::Update>();
    c.SetSchedulerAfter<TestScheduler, Engine::Scheduler::FixedTimeUpdate>();
    c.SetSchedulerBefore<TestScheduler, Engine::Scheduler::Shutdown>();

    c.RegisterSystem<Engine::Scheduler::Update>(
        [&](Engine::Core &core) { core.GetResource<Engine::Resource::Time>()._elapsedTime = 2.0f; });

    c.AddPlugins<Physics::Plugin>();

    auto player = c.CreateEntity();
    float startY = 10.0f;

    c.RegisterSystem<Engine::Scheduler::Startup>([&](Engine::Core & /*core*/) {
        player.AddComponent<Object::Component::Transform>(glm::vec3(0.0f, startY, 0.0f));
        player.AddComponent<Physics::Component::CapsuleCollider>(0.8f, 0.3f);
        player.AddComponent<Physics::Component::CharacterController>();
    });

    c.RegisterSystem<TestScheduler>([&](Engine::Core & /*core*/) {
        const auto &transform = player.GetComponents<Object::Component::Transform>();
        EXPECT_LT(transform.GetPosition().y, startY);
    });

    c.RunSystems();
}

TEST(CharacterControllerPlugin, CharacterLandsOnFloor)
{
    Engine::Core c;

    c.RegisterScheduler<TestScheduler>();
    c.SetSchedulerAfter<TestScheduler, Engine::Scheduler::Update>();
    c.SetSchedulerAfter<TestScheduler, Engine::Scheduler::FixedTimeUpdate>();
    c.SetSchedulerBefore<TestScheduler, Engine::Scheduler::Shutdown>();

    c.RegisterSystem<Engine::Scheduler::Update>(
        [&](Engine::Core &core) { core.GetResource<Engine::Resource::Time>()._elapsedTime = 3.0f; });

    c.AddPlugins<Physics::Plugin>();

    auto player = c.CreateEntity();
    auto floor = c.CreateEntity();
    float startY = 5.0f;

    c.RegisterSystem<Engine::Scheduler::Startup>([&](Engine::Core & /*core*/) {
        floor.AddComponent<Object::Component::Transform>(glm::vec3(0.0f, 0.0f, 0.0f));
        floor.AddComponent<Physics::Component::BoxCollider>(glm::vec3(50.0f, 0.5f, 50.0f));
        floor.AddComponent<Physics::Component::RigidBody>(Physics::Component::RigidBody::CreateStatic());

        player.AddComponent<Object::Component::Transform>(glm::vec3(0.0f, startY, 0.0f));
        player.AddComponent<Physics::Component::CapsuleCollider>(0.8f, 0.3f);
        player.AddComponent<Physics::Component::CharacterController>();
    });

    c.RegisterSystem<TestScheduler>([&](Engine::Core & /*core*/) {
        const auto &transform = player.GetComponents<Object::Component::Transform>();
        const float y = transform.GetPosition().y;

        EXPECT_LT(y, startY);

        EXPECT_GT(y, -1.0f);
    });

    c.RunSystems();
}

TEST(CharacterControllerPlugin, DefaultSettings)
{
    Physics::Component::CharacterController cc;
    EXPECT_FLOAT_EQ(cc.mass, 70.0f);
    EXPECT_FLOAT_EQ(cc.gravityFactor, 1.0f);
    EXPECT_GT(cc.maxSlopeAngle, 0.0f);
    EXPECT_GT(cc.maxStepHeight, 0.0f);
    EXPECT_EQ(cc.linearVelocity, glm::vec3(0.0f));
}
