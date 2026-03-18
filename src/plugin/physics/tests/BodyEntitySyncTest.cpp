#include <gtest/gtest.h>

#include "component/BoxCollider.hpp"
#include "component/RigidBody.hpp"
#include "component/RigidBodyInternal.hpp"
#include "component/Transform.hpp"
#include "core/Core.hpp"
#include "entity/Entity.hpp"
#include "plugin/PhysicsPlugin.hpp"
#include "resource/BodyEntityMap.hpp"
#include "scheduler/Startup.hpp"

TEST(PhysicsPlugin, BodyEntityMapAddition)
{
    Engine::Core core;

    core.SetErrorPolicyForAllSchedulers(Engine::Scheduler::SchedulerErrorPolicy::Nothing);

    core.AddPlugins<Physics::Plugin>();

    core.RegisterSystem<Engine::Scheduler::Startup>([&](Engine::Core &core) {
        auto entity1 = core.CreateEntity();
        auto entity2 = core.CreateEntity();
        auto entity3 = core.CreateEntity();

        auto &bodyEntityMap = core.GetResource<Physics::Resource::BodyEntityMap>();

        EXPECT_EQ(bodyEntityMap.Size(), 0);

        entity1.AddComponent<Object::Component::Transform>();
        entity1.AddComponent<Physics::Component::BoxCollider>();
        entity1.AddComponent<Physics::Component::RigidBody>();
        entity2.AddComponent<Object::Component::Transform>();
        entity2.AddComponent<Physics::Component::BoxCollider>();
        entity2.AddComponent<Physics::Component::RigidBody>();
        entity3.AddComponent<Object::Component::Transform>();
        entity3.AddComponent<Physics::Component::BoxCollider>();
        entity3.AddComponent<Physics::Component::RigidBody>();

        EXPECT_EQ(bodyEntityMap.Size(), 3);
    });

    core.RunSystems();
}

TEST(PhysicsPlugin, BodyEntityMapRemoval)
{
    Engine::Core core;

    core.AddPlugins<Physics::Plugin>();

    core.RegisterSystem<Engine::Scheduler::Startup>([&](Engine::Core &core) {
        auto entity1 = core.CreateEntity();
        auto entity2 = core.CreateEntity();
        auto entity3 = core.CreateEntity();

        auto &bodyEntityMap = core.GetResource<Physics::Resource::BodyEntityMap>();

        EXPECT_EQ(bodyEntityMap.Size(), 0);

        entity1.AddComponent<Object::Component::Transform>();
        entity1.AddComponent<Physics::Component::BoxCollider>();
        entity1.AddComponent<Physics::Component::RigidBody>();
        entity2.AddComponent<Object::Component::Transform>();
        entity2.AddComponent<Physics::Component::BoxCollider>();
        entity2.AddComponent<Physics::Component::RigidBody>();
        entity3.AddComponent<Object::Component::Transform>();
        entity3.AddComponent<Physics::Component::BoxCollider>();
        entity3.AddComponent<Physics::Component::RigidBody>();

        EXPECT_EQ(bodyEntityMap.Size(), 3);

        entity1.Kill();

        EXPECT_EQ(bodyEntityMap.Size(), 2);

        entity2.Kill();

        EXPECT_EQ(bodyEntityMap.Size(), 1);

        entity3.Kill();

        EXPECT_EQ(bodyEntityMap.Size(), 0);
    });
    core.RunSystems();
}

TEST(PhysicsPlugin, BodyEntityMapRetrieval)
{
    Engine::Core core;

    core.SetErrorPolicyForAllSchedulers(Engine::Scheduler::SchedulerErrorPolicy::Nothing);

    core.AddPlugins<Physics::Plugin>();

    core.RegisterSystem<Engine::Scheduler::Startup>([&](Engine::Core &core) {
        std::array<Engine::Entity, 3> entities{core.CreateEntity(), core.CreateEntity(), core.CreateEntity()};

        auto &bodyEntityMap = core.GetResource<Physics::Resource::BodyEntityMap>();

        for (auto &entity : entities)
        {
            entity.AddComponent<Object::Component::Transform>();
            entity.AddComponent<Physics::Component::BoxCollider>();
            entity.AddComponent<Physics::Component::RigidBody>();
        }

        for (const auto &entity : entities)
        {
            EXPECT_EQ(bodyEntityMap.Get(entity), entity.GetComponents<Physics::Component::RigidBodyInternal>().bodyID);
            EXPECT_EQ(entity, bodyEntityMap.Get(entity.GetComponents<Physics::Component::RigidBodyInternal>().bodyID));
        }
    });
    core.RunSystems();
}

TEST(PhysicsPlugin, BodyEntityMapErroneousRetrieval)
{
    Engine::Core core;

    core.AddPlugins<Physics::Plugin>();

    core.RegisterSystem<Engine::Scheduler::Startup>([&](Engine::Core &core) {
        Engine::Entity entityWithComponents = core.CreateEntity();
        Engine::Entity entityWithoutComponents = core.CreateEntity();

        auto &bodyEntityMap = core.GetResource<Physics::Resource::BodyEntityMap>();

        entityWithComponents.AddComponent<Object::Component::Transform>();
        entityWithComponents.AddComponent<Physics::Component::BoxCollider>();
        entityWithComponents.AddComponent<Physics::Component::RigidBody>();

        EXPECT_EQ(bodyEntityMap.Contains(entityWithoutComponents), false);
        EXPECT_EQ(bodyEntityMap.Contains(entityWithComponents), true);
        EXPECT_EQ(bodyEntityMap.Size(), 1);
    });
    core.RunSystems();
}
