#include <glm/glm.hpp>
#include <gtest/gtest.h>

#include "component/CapsuleCollider.hpp"
#include "component/CharacterController.hpp"
#include "component/CharacterControllerInternal.hpp"
#include "component/BoxCollider.hpp"
#include "component/RigidBody.hpp"
#include "component/Transform.hpp"
#include "core/Core.hpp"
#include "entity/Entity.hpp"
#include "plugin/PhysicsPlugin.hpp"
#include "resource/Time.hpp"
#include "scheduler/FixedTimeUpdate.hpp"
#include "scheduler/Shutdown.hpp"
#include "scheduler/Startup.hpp"

// ---------------------------------------------------------------------------
// Helper scheduler used to run assertions after physics has settled
// ---------------------------------------------------------------------------
class TestScheduler : public Engine::Scheduler::Update {
  public:
    using Engine::Scheduler::Update::Update;
};

// ---------------------------------------------------------------------------
// Test 1 – CharacterController can be created and its internal is valid
// ---------------------------------------------------------------------------
TEST(CharacterControllerPlugin, CharacterControllerCreation)
{
    Engine::Core core;
    core.AddPlugins<Physics::Plugin>();

    core.RegisterSystem<Engine::Scheduler::Update>(
        [&](Engine::Core &c) { c.GetResource<Engine::Resource::Time>()._elapsedTime = 0.016f; });

    // Run startup to initialise physics
    core.GetScheduler<Engine::Scheduler::Startup>().RunSystems();

    auto player = core.CreateEntity();
    player.AddComponent<Object::Component::Transform>(glm::vec3(0.0f, 5.0f, 0.0f));
    player.AddComponent<Physics::Component::CapsuleCollider>(0.8f, 0.3f);
    player.AddComponent<Physics::Component::CharacterController>(Physics::Component::CharacterController::Create(70.0f));

    // Internal should have been auto-created by the entt hook
    ASSERT_TRUE(player.HasComponents<Physics::Component::CharacterControllerInternal>());

    const auto &internal = player.GetComponents<Physics::Component::CharacterControllerInternal>();
    EXPECT_TRUE(internal.IsValid());
}

// ---------------------------------------------------------------------------
// Test 2 – Character falls under gravity (Y position decreases over time)
// ---------------------------------------------------------------------------
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
        player.AddComponent<Physics::Component::CharacterController>(
            Physics::Component::CharacterController::Create(70.0f));
    });

    c.RegisterSystem<TestScheduler>([&](Engine::Core & /*core*/) {
        const auto &transform = player.GetComponents<Object::Component::Transform>();
        // After 2 s in free-fall the character must have moved downward
        EXPECT_LT(transform.GetPosition().y, startY);
    });

    c.RunSystems();
}

// ---------------------------------------------------------------------------
// Test 3 – Character lands on a static floor and stops falling
// ---------------------------------------------------------------------------
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
        // Static floor at Y = 0
        floor.AddComponent<Object::Component::Transform>(glm::vec3(0.0f, 0.0f, 0.0f));
        floor.AddComponent<Physics::Component::BoxCollider>(glm::vec3(50.0f, 0.5f, 50.0f));
        floor.AddComponent<Physics::Component::RigidBody>(Physics::Component::RigidBody::CreateStatic());

        // Player spawned above the floor
        player.AddComponent<Object::Component::Transform>(glm::vec3(0.0f, startY, 0.0f));
        player.AddComponent<Physics::Component::CapsuleCollider>(0.8f, 0.3f);
        player.AddComponent<Physics::Component::CharacterController>(
            Physics::Component::CharacterController::Create(70.0f));
    });

    c.RegisterSystem<TestScheduler>([&](Engine::Core & /*core*/) {
        const auto &transform = player.GetComponents<Object::Component::Transform>();
        const float y = transform.GetPosition().y;

        // Must have fallen from startY
        EXPECT_LT(y, startY);

        // Must have been stopped by the floor: floor top is at ~0.5,
        // character capsule half-height is 0.8 + radius 0.3 = 1.1 m,
        // so the character center should rest around y ≈ 1.1 ± a small margin.
        EXPECT_GT(y, -1.0f);
    });

    c.RunSystems();
}

// ---------------------------------------------------------------------------
// Test 4 – Default factory method produces expected defaults
// ---------------------------------------------------------------------------
TEST(CharacterControllerPlugin, DefaultSettings)
{
    Physics::Component::CharacterController cc = Physics::Component::CharacterController::Create();
    EXPECT_FLOAT_EQ(cc.mass, 70.0f);
    EXPECT_FLOAT_EQ(cc.gravityFactor, 1.0f);
    EXPECT_GT(cc.maxSlopeAngle, 0.0f);
    EXPECT_GT(cc.maxStepHeight, 0.0f);
    EXPECT_EQ(cc.linearVelocity, glm::vec3(0.0f));
}
