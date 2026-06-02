#include <glm/glm.hpp>
#include <gtest/gtest.h>

#include "component/BoxCollider.hpp"
#include "component/RigidBody.hpp"
#include "component/Transform.hpp"
#include "core/Core.hpp"
#include "entity/Entity.hpp"
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

struct CollisionState {
    bool added = false;
    bool removed = false;
};

class CubeFallingOnPlanePlugin : public Engine::APlugin {
  public:
    explicit CubeFallingOnPlanePlugin(Engine::Core &core) : Engine::APlugin(core) {}
    ~CubeFallingOnPlanePlugin() = default;

    void Bind() final
    {
        RequirePlugins<Physics::Plugin>();
        RequirePlugins<TestSchedulerSetupPlugin>();

        RegisterSystems<Engine::Scheduler::Startup>([](Engine::Core &core) {
            auto plane = core.CreateEntity();
            plane.AddComponent<Object::Component::Transform>();
            plane.AddComponent<Physics::Component::BoxCollider>(glm::vec3(50.0f, 1.0f, 50.0f));
            plane.AddComponent<Physics::Component::RigidBody>(Physics::Component::RigidBody::CreateStatic());

            auto cubeEntity = core.CreateEntity();
            cubeEntity.AddComponent<Object::Component::Transform>(glm::vec3(0.0f, 10.0f, 0.0f));
            cubeEntity.AddComponent<Physics::Component::BoxCollider>(glm::vec3(0.5f, 0.5f, 0.5f));
            cubeEntity.AddComponent<Physics::Component::RigidBody>(Physics::Component::RigidBody::CreateDynamic());

            core.RegisterResource(std::move(cubeEntity));
            core.RegisterResource(CollisionState{});

            auto cube = core.GetResource<Engine::Entity>();
            auto &eventManager = core.GetResource<Event::Resource::EventManager>();
            auto &state = core.GetResource<CollisionState>();

            eventManager.RegisterCallback<Physics::Event::CollisionAddedEvent>(
                [cube, plane, &state](const Physics::Event::CollisionAddedEvent &event) {
                    if ((event.entity1 == cube && event.entity2 == plane) ||
                        (event.entity1 == plane && event.entity2 == cube))
                    {
                        state.added = true;
                    }
                });

            eventManager.RegisterCallback<Physics::Event::CollisionRemovedEvent>(
                [cube, plane, &state](const Physics::Event::CollisionRemovedEvent &event) {
                    if ((event.entity1 == cube && event.entity2 == plane) ||
                        (event.entity1 == plane && event.entity2 == cube))
                    {
                        state.removed = true;
                    }
                });
        });

        RegisterSystems<Engine::Scheduler::Update>(
            [](Engine::Core &core) { core.GetResource<Engine::Resource::Time>()._elapsedTime = 3.2f; });

        const float startY = 10.0f;
        RegisterSystems<Example::TestScheduler>([startY](Engine::Core &core) {
            auto cube = core.GetResource<Engine::Entity>();
            const auto &cubeTransform = cube.GetComponents<Object::Component::Transform>();
            EXPECT_LT(cubeTransform.GetPosition().y, startY);

            const auto &state = core.GetResource<CollisionState>();
            EXPECT_TRUE(state.added);
            EXPECT_TRUE(state.removed);
        });
    }
};

} // namespace Example

TEST(PluginPhysics, CubeFallingOnPlane)
{
    Engine::Core c;

    c.AddPlugins<Example::CubeFallingOnPlanePlugin>();

    c.RunSystems();
}
