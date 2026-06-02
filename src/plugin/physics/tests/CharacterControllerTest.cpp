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

namespace Example {
class TestScheduler : public Engine::Scheduler::Update {
  public:
    using Engine::Scheduler::Update::Update;
};

class TestSchedulerSetupPlugin : public Engine::APlugin {
  public:
    explicit TestSchedulerSetupPlugin(Engine::Core &core)
        : Engine::APlugin(core) {
              // empty
          };
    ~TestSchedulerSetupPlugin() = default;

    void Bind() final
    {
        auto &core = GetCore();
        core.RegisterScheduler<Example::TestScheduler>();
        core.SetSchedulerAfter<Example::TestScheduler, Engine::Scheduler::Update>();
        core.SetSchedulerAfter<Example::TestScheduler, Engine::Scheduler::FixedTimeUpdate>();
        core.SetSchedulerBefore<Example::TestScheduler, Engine::Scheduler::Shutdown>();
    }
};

class CharacterControllerCreationPlugin : public Engine::APlugin {
  public:
    explicit CharacterControllerCreationPlugin(Engine::Core &core)
        : Engine::APlugin(core) {
              // empty
          };
    ~CharacterControllerCreationPlugin() = default;

    void Bind() final
    {
        RequirePlugins<Physics::Plugin>();

        RegisterSystems<Engine::Scheduler::Startup>([](Engine::Core &core) {
            auto player = core.CreateEntity();
            player.AddComponent<Object::Component::Transform>(glm::vec3(0.0f, 5.0f, 0.0f));
            player.AddComponent<Physics::Component::CapsuleCollider>(0.8f, 0.3f);
            player.AddComponent<Physics::Component::CharacterController>();

            ASSERT_TRUE(player.HasComponents<Physics::Component::CharacterControllerInternal>());

            const auto &internal = player.GetComponents<Physics::Component::CharacterControllerInternal>();
            EXPECT_TRUE(internal.IsValid());
        });
    }
};

class CharacterFallsUnderGravityPlugin : public Engine::APlugin {
  public:
    explicit CharacterFallsUnderGravityPlugin(Engine::Core &core)
        : Engine::APlugin(core) {
              // empty
          };
    ~CharacterFallsUnderGravityPlugin() = default;

    void Bind() final
    {
        RequirePlugins<Physics::Plugin>();
        RequirePlugins<TestSchedulerSetupPlugin>();

        float startY = 10.f;
        RegisterSystems<Engine::Scheduler::Startup>([startY](Engine::Core &core) {
            auto player = core.RegisterResource(core.CreateEntity());
            player.AddComponent<Object::Component::Transform>(glm::vec3(0.0f, 10.f, 0.0f));
            player.AddComponent<Physics::Component::CapsuleCollider>(0.8f, 0.3f);
            player.AddComponent<Physics::Component::CharacterController>();
        });

        RegisterSystems<Engine::Scheduler::Update>(
            [&](Engine::Core &core) { core.GetResource<Engine::Resource::Time>()._elapsedTime = 2.0f; });

        RegisterSystems<Example::TestScheduler>([startY](Engine::Core &core) {
            auto player = core.GetResource<Engine::Entity>();
            const auto &transform = player.GetComponents<Object::Component::Transform>();
            EXPECT_LT(transform.GetPosition().y, startY);
        });
    }
};

class CharacterLandsOnFloorPlugin : public Engine::APlugin {
  public:
    explicit CharacterLandsOnFloorPlugin(Engine::Core &core)
        : Engine::APlugin(core) {
              // empty
          };
    ~CharacterLandsOnFloorPlugin() = default;

    void Bind() final
    {
        RequirePlugins<Physics::Plugin>();
        RequirePlugins<TestSchedulerSetupPlugin>();

        RegisterSystems<Engine::Scheduler::Update>(
            [&](Engine::Core &core) { core.GetResource<Engine::Resource::Time>()._elapsedTime = 2.0f; });

        float startY = 5.0f;
        RegisterSystems<Engine::Scheduler::Startup>([startY](Engine::Core &core) {
            auto player = core.RegisterResource(core.CreateEntity());
            auto floor = core.CreateEntity();
            floor.AddComponent<Object::Component::Transform>(glm::vec3(0.0f, 0.0f, 0.0f));
            floor.AddComponent<Physics::Component::BoxCollider>(glm::vec3(50.0f, 0.5f, 50.0f));
            floor.AddComponent<Physics::Component::RigidBody>(Physics::Component::RigidBody::CreateStatic());

            player.AddComponent<Object::Component::Transform>(glm::vec3(0.0f, startY, 0.0f));
            player.AddComponent<Physics::Component::CapsuleCollider>(0.8f, 0.3f);
            player.AddComponent<Physics::Component::CharacterController>();
        });

        RegisterSystems<Example::TestScheduler>([startY](Engine::Core &core) {
            auto player = core.GetResource<Engine::Entity>();
            const auto &transform = player.GetComponents<Object::Component::Transform>();
            const float y = transform.GetPosition().y;
            EXPECT_LT(y, startY);
            EXPECT_GT(y, -1.0f);
        });
    }
};

} // namespace Example

TEST(CharacterControllerPlugin, CharacterControllerCreation)
{
    Engine::Core core;

    core.SetErrorPolicyForAllSchedulers(Engine::Scheduler::SchedulerErrorPolicy::Nothing);

    core.AddPlugins<Example::CharacterControllerCreationPlugin>();
}

TEST(CharacterControllerPlugin, CharacterFallsUnderGravity)
{
    Engine::Core c;

    c.AddPlugins<Example::CharacterFallsUnderGravityPlugin>();

    c.RunSystems();
}

TEST(CharacterControllerPlugin, CharacterLandsOnFloor)
{
    Engine::Core c;

    c.AddPlugins<Example::CharacterLandsOnFloorPlugin>();

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
