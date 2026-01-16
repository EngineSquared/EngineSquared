#include <glm/glm.hpp>
#include <gtest/gtest.h>

#include "component/BoxCollider.hpp"
#include "component/RigidBody.hpp"
#include "component/Transform.hpp"
#include "core/Core.hpp"
#include "event/CollisionEvent.hpp"
#include "plugin/PhysicsPlugin.hpp"
#include "plugin/PluginEvent.hpp"
#include "resource/EventManager.hpp"
#include "resource/Time.hpp"

#include "utils/Layers.hpp"
#include <thread>

TEST(PhysicsPlugin, CubeFallingOnPlane)
{
    Engine::Core core;

    core.RegisterSystem<Engine::Scheduler::Update>(
        [&](Engine::Core &c) { c.GetResource<Engine::Resource::Time>()._elapsedTime = 0.016f; });

    core.AddPlugins<Physics::Plugin>();
    core.RunSystems();

    auto plane = Engine::Entity{core, core.CreateEntity()};
    {
        Object::Component::Transform transform;
        transform.SetPosition(0.0f, 0.0f, 0.0f);
        plane.AddComponent<Object::Component::Transform>(transform);

        Physics::Component::RigidBody rigidBody;
        rigidBody.motionType = Physics::Component::MotionType::Static;
        rigidBody.objectLayer = Physics::Utils::Layers::NON_MOVING;
        rigidBody.restitution = 1.0f;
        plane.AddComponent<Physics::Component::RigidBody>(rigidBody);

        Physics::Component::BoxCollider collider;
        collider.halfExtents = glm::vec3(50.0f, 1.0f, 50.0f);
        plane.AddComponent<Physics::Component::BoxCollider>(collider);
    }

    auto cube = Engine::Entity{core, core.CreateEntity()};
    float startY = 10.0f;
    {
        Object::Component::Transform transform;
        transform.SetPosition(0.0f, startY, 0.0f);
        cube.AddComponent<Object::Component::Transform>(transform);

        Physics::Component::RigidBody rigidBody;
        rigidBody.objectLayer = Physics::Utils::Layers::MOVING;
        rigidBody.motionType = Physics::Component::MotionType::Dynamic;
        rigidBody.mass = 1.0f;
        rigidBody.restitution = 1.0f;
        cube.AddComponent<Physics::Component::RigidBody>(rigidBody);

        Physics::Component::BoxCollider collider;
        collider.halfExtents = glm::vec3(0.5f, 0.5f, 0.5f);
        cube.AddComponent<Physics::Component::BoxCollider>(collider);
    }

    bool collisionAdded = false;
    bool collisionRemoved = false;

    core.GetResource<Event::Resource::EventManager>().RegisterCallback<Physics::Event::CollisionAddedEvent>(
        [&](const Physics::Event::CollisionAddedEvent &event) {
            if ((event.entity1 == cube && event.entity2 == plane) || (event.entity1 == plane && event.entity2 == cube))
            {
                collisionAdded = true;
            }
        });

    core.GetResource<Event::Resource::EventManager>().RegisterCallback<Physics::Event::CollisionRemovedEvent>(
        [&](const Physics::Event::CollisionRemovedEvent &event) {
            if ((event.entity1 == cube && event.entity2 == plane) || (event.entity1 == plane && event.entity2 == cube))
            {
                collisionRemoved = true;
            }
        });

    for (int i = 0; i < 200; ++i)
    {
        core.RunSystems();
    }

    const auto &cubeTransform = cube.GetComponents<Object::Component::Transform>();
    EXPECT_LT(cubeTransform.GetPosition().y, startY);
    EXPECT_TRUE(collisionAdded);
    EXPECT_TRUE(collisionRemoved);
}
