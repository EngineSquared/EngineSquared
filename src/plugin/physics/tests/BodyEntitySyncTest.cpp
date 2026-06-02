#include <gtest/gtest.h>

#include "component/BoxCollider.hpp"
#include "component/RigidBody.hpp"
#include "component/RigidBodyInternal.hpp"
#include "component/Transform.hpp"
#include "core/Core.hpp"
#include "entity/Entity.hpp"
#include "plugin/PluginPhysics.hpp"
#include "resource/BodyEntityMap.hpp"
#include "scheduler/Startup.hpp"

namespace Example {
class BodyEntityMapAdditionPlugin : public Engine::APlugin {
  public:
    explicit BodyEntityMapAdditionPlugin(Engine::Core &core)
        : Engine::APlugin(core) {
              // empty
          };
    ~BodyEntityMapAdditionPlugin() = default;

    void Bind() final
    {
        RequirePlugins<Physics::Plugin>();
        RegisterSystems<Engine::Scheduler::Startup>([&](Engine::Core &core) {
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
    }
};
} // namespace Example

TEST(PluginPhysics, BodyEntityMapAddition)
{
    Engine::Core c;

    c.SetErrorPolicyForAllSchedulers(Engine::Scheduler::SchedulerErrorPolicy::Nothing);

    c.AddPlugins<Example::BodyEntityMapAdditionPlugin>();

    c.RunSystems();
}

namespace Example {
class BodyEntityMapRemovalPlugin : public Engine::APlugin {
  public:
    explicit BodyEntityMapRemovalPlugin(Engine::Core &core)
        : Engine::APlugin(core) {
              // empty
          };
    ~BodyEntityMapRemovalPlugin() = default;

    void Bind() final
    {
        RequirePlugins<Physics::Plugin>();
        RegisterSystems<Engine::Scheduler::Startup>([&](Engine::Core &core) {
            std::array<Engine::Entity, 3> entities{core.CreateEntity(), core.CreateEntity(), core.CreateEntity()};

            auto &bodyEntityMap = core.GetResource<Physics::Resource::BodyEntityMap>();

            EXPECT_EQ(bodyEntityMap.Size(), 0);

            for (auto &entity : entities)
            {
                entity.AddComponent<Object::Component::Transform>();
                entity.AddComponent<Physics::Component::BoxCollider>();
                entity.AddComponent<Physics::Component::RigidBody>();
            }

            for (std::uint64_t cursor = 0; cursor < entities.size(); cursor++)
            {
                EXPECT_EQ(bodyEntityMap.Size(), entities.size() - cursor);
                entities[cursor].Kill();
            }
        });
    }
};
} // namespace Example

TEST(PluginPhysics, BodyEntityMapRemoval)
{
    Engine::Core c;

    c.SetErrorPolicyForAllSchedulers(Engine::Scheduler::SchedulerErrorPolicy::Nothing);

    c.AddPlugins<Example::BodyEntityMapRemovalPlugin>();
    c.RunSystems();
}
namespace Example {
class BodyEntityMapRetrievalPlugin : public Engine::APlugin {
  public:
    explicit BodyEntityMapRetrievalPlugin(Engine::Core &core)
        : Engine::APlugin(core) {
              // empty
          };
    ~BodyEntityMapRetrievalPlugin() = default;

    void Bind() final
    {
        RequirePlugins<Physics::Plugin>();
        RegisterSystems<Engine::Scheduler::Startup>([&](Engine::Core &core) {
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
                EXPECT_EQ(bodyEntityMap.Get(entity),
                          entity.GetComponents<Physics::Component::RigidBodyInternal>().bodyID);
                EXPECT_EQ(entity,
                          bodyEntityMap.Get(entity.GetComponents<Physics::Component::RigidBodyInternal>().bodyID));
            }
        });
    }
};
} // namespace Example

TEST(PluginPhysics, BodyEntityMapRetrieval)
{
    Engine::Core c;

    c.SetErrorPolicyForAllSchedulers(Engine::Scheduler::SchedulerErrorPolicy::Nothing);

    c.AddPlugins<Example::BodyEntityMapRetrievalPlugin>();

    c.RunSystems();
}

namespace Example {
class BodyEntityMapErroneousRetrievalPlugin : public Engine::APlugin {
  public:
    explicit BodyEntityMapErroneousRetrievalPlugin(Engine::Core &core)
        : Engine::APlugin(core) {
              // empty
          };
    ~BodyEntityMapErroneousRetrievalPlugin() = default;

    void Bind() final
    {
        RequirePlugins<Physics::Plugin>();
        RegisterSystems<Engine::Scheduler::Startup>([&](Engine::Core &core) {
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
    }
};
} // namespace Example

TEST(PluginPhysics, BodyEntityMapErroneousRetrieval)
{
    Engine::Core c;

    c.SetErrorPolicyForAllSchedulers(Engine::Scheduler::SchedulerErrorPolicy::Nothing);

    c.AddPlugins<Example::BodyEntityMapErroneousRetrievalPlugin>();
    c.RunSystems();
}
