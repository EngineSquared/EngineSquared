#include <gtest/gtest.h>

#include <glm/glm.hpp>

#include "component/BoxCollider.hpp"
#include "component/RigidBody.hpp"
#include "component/Transform.hpp"
#include "core/Core.hpp"
#include "entity/Entity.hpp"
#include "plugin/PluginPhysics.hpp"
#include "scheduler/Startup.hpp"
#include "utils/CastRay.hpp"
#include "utils/Ray.hpp"

namespace {
void ConfigurePhysicsCore(Engine::Core &core)
{
    core.SetErrorPolicyForAllSchedulers(Engine::Scheduler::SchedulerErrorPolicy::Nothing);
    core.AddPlugins<Physics::Plugin>();
}
} // namespace

TEST(CastRay, NoHit)
{
    Engine::Core core;
    ConfigurePhysicsCore(core);

    core.RegisterSystem<Engine::Scheduler::Startup>([&](Engine::Core &) {
        const auto hit = Physics::Utils::CastRay(
            core, Physics::Utils::Ray{glm::vec3(-2.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)}, 4.0f);
        EXPECT_FALSE(hit.has_value());
    });

    core.RunSystems();
}

TEST(CastRay, Hit)
{
    Engine::Core core;
    ConfigurePhysicsCore(core);

    core.RegisterSystem<Engine::Scheduler::Startup>([](Engine::Core &core) {
        auto entity = core.CreateEntity();

        entity.AddComponent<Object::Component::Transform>(glm::vec3(0.0f, 0.0f, 0.0f));

        auto boxCollider = Physics::Component::BoxCollider(glm::vec3(0.5f, 0.5f, 0.5f));
        boxCollider.convexRadius = 0.0f;
        entity.AddComponent<Physics::Component::BoxCollider>(std::move(boxCollider));
        entity.AddComponent<Physics::Component::RigidBody>(Physics::Component::RigidBody::CreateStatic());

        const auto hit = Physics::Utils::CastRay(
            core, Physics::Utils::Ray{glm::vec3(-2.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)}, 4.0f);

        ASSERT_TRUE(hit.has_value());
        EXPECT_EQ(hit->hitEntityId, entity.Id());
        EXPECT_NEAR(hit->t, 0.375f, 0.05f);
    });

    core.RunSystems();
}
