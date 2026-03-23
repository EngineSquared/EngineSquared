#include <glm/glm.hpp>
#include <gtest/gtest.h>

#include "component/BoxCollider.hpp"
#include "component/RigidBody.hpp"
#include "component/Transform.hpp"
#include "core/Core.hpp"
#include "event/CollisionEvent.hpp"
#include "plugin/PluginEvent.hpp"
#include "plugin/PluginPhysics.hpp"
#include "resource/EventManager.hpp"
#include "resource/Time.hpp"
#include "scheduler/FixedTimeUpdate.hpp"
#include "scheduler/Shutdown.hpp"
#include "scheduler/Startup.hpp"

#include "utils/Layers.hpp"
#include <thread>

class TestScheduler : public Engine::Scheduler::Update {
  public:
    using Engine::Scheduler::Update::Update;
};

TEST(PluginPhysics, CubeFallingOnPlane)
{
    Engine::Core c;

    c.RegisterScheduler<TestScheduler>();
    c.SetSchedulerAfter<TestScheduler, Engine::Scheduler::Update>();
    c.SetSchedulerAfter<TestScheduler, Engine::Scheduler::FixedTimeUpdate>();
    c.SetSchedulerBefore<TestScheduler, Engine::Scheduler::Shutdown>();

    c.RegisterSystem<Engine::Scheduler::Update>(
        [&](Engine::Core &c) { c.GetResource<Engine::Resource::Time>()._elapsedTime = 3.2f; });

    c.AddPlugins<Physics::Plugin>();

    auto cube = c.CreateEntity();
    auto plane = c.CreateEntity();
    float startY = 10.0f;
    bool collisionAdded = false;
    bool collisionRemoved = false;

    c.RegisterSystem<Engine::Scheduler::Startup>([&](Engine::Core &core) {
        plane.AddComponent<Object::Component::Transform>();
        plane.AddComponent<Physics::Component::BoxCollider>(glm::vec3(50.0f, 1.0f, 50.0f));
        plane.AddComponent<Physics::Component::RigidBody>(Physics::Component::RigidBody::CreateStatic());

        cube.AddComponent<Object::Component::Transform>(glm::vec3(0.0f, startY, 0.0f));
        cube.AddComponent<Physics::Component::BoxCollider>(glm::vec3(0.5f, 0.5f, 0.5f));
        cube.AddComponent<Physics::Component::RigidBody>(Physics::Component::RigidBody::CreateDynamic());

        auto &eventManager = core.GetResource<Event::Resource::EventManager>();
        eventManager.RegisterCallback<Physics::Event::CollisionAddedEvent>(
            [&](const Physics::Event::CollisionAddedEvent &event) {
                if ((event.entity1 == cube && event.entity2 == plane) ||
                    (event.entity1 == plane && event.entity2 == cube))
                {
                    collisionAdded = true;
                }
            });

        eventManager.RegisterCallback<Physics::Event::CollisionRemovedEvent>(
            [&](const Physics::Event::CollisionRemovedEvent &event) {
                if ((event.entity1 == cube && event.entity2 == plane) ||
                    (event.entity1 == plane && event.entity2 == cube))
                {
                    collisionRemoved = true;
                }
            });
    });
    c.RegisterSystem<TestScheduler>([&](Engine::Core &core) {
        const auto &cubeTransform = cube.GetComponents<Object::Component::Transform>();
        EXPECT_LT(cubeTransform.GetPosition().y, startY);
        EXPECT_TRUE(collisionAdded);
        EXPECT_TRUE(collisionRemoved);
    });
    c.RunSystems();
}
