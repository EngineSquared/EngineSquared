#include <gtest/gtest.h>

#include "ABABCollision3D.hpp"
#include "BoxCollider3D.hpp"
#include "CollisionCheckerABAB.hpp"
#include "Core.hpp"
#include "Entity.hpp"
#include "Transform.hpp"

using namespace ES::Plugin::Physics;

TEST(Collision, CollisionSystemWithBoxCollider3D)
{
    ES::Engine::Core core;

    ES::Engine::Entity eA = core.CreateEntity();
    ES::Engine::Entity eB = core.CreateEntity();

    eA.AddComponent<ES::Plugin::Physics::Component::BoxCollider3D>(core, glm::vec3(1, 1, 1));
    eB.AddComponent<ES::Plugin::Physics::Component::BoxCollider3D>(core, glm::vec3(1, 1, 1));
    eA.AddComponent<ES::Plugin::Object::Component::Transform>(core, glm::vec3(1, 1, 1));
    eB.AddComponent<ES::Plugin::Object::Component::Transform>(core, glm::vec3(1, 1, 1));

    core.RegisterSystem(ES::Plugin::Physics::System::RemoveABABCollisions);
    core.RegisterSystem(ES::Plugin::Physics::System::DetectABABCollisions);

    core.RunSystems();

    auto view = core.GetRegistry().view<ES::Plugin::Physics::Component::ABABCollision3D>();

    EXPECT_EQ(view.size(), 1);

    eA.GetComponents<ES::Plugin::Object::Component::Transform>(core).setPosition(9, 9, 9);

    core.RunSystems();

    view = core.GetRegistry().view<ES::Plugin::Physics::Component::ABABCollision3D>();

    EXPECT_EQ(view.size(), 0);
}
