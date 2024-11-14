#include <gtest/gtest.h>

#include "ABABCollision3D.hpp"
#include "BoxCollider3D.hpp"
#include "CollisionCheckerABAB.hpp"
#include "Entity.hpp"
#include "Registry.hpp"
#include "Transform.hpp"

using namespace ES::Plugin::Physics;

TEST(Collision, CollisionSystemWithBoxCollider3D)
{
    ES::Engine::Registry registry;

    ES::Engine::Entity eA(registry.CreateEntity());
    ES::Engine::Entity eB(registry.CreateEntity());

    eA.AddComponent<ES::Plugin::Physics::Component::BoxCollider3D>(registry, glm::vec3(1, 1, 1));
    eB.AddComponent<ES::Plugin::Physics::Component::BoxCollider3D>(registry, glm::vec3(1, 1, 1));
    eA.AddComponent<ES::Plugin::Object::Component::Transform>(registry, glm::vec3(1, 1, 1));
    eB.AddComponent<ES::Plugin::Object::Component::Transform>(registry, glm::vec3(1, 1, 1));

    registry.RegisterSystem(ES::Plugin::Physics::System::RemoveABABCollisions);
    registry.RegisterSystem(ES::Plugin::Physics::System::DetectABABCollisions);

    registry.RunSystems();

    auto view = registry.GetRegistry().view<ES::Plugin::Physics::Component::ABABCollision3D>();

    EXPECT_EQ(view.size(), 1);

    eA.GetComponents<ES::Plugin::Object::Component::Transform>(registry).setPosition(9, 9, 9);

    registry.RunSystems();

    view = registry.GetRegistry().view<ES::Plugin::Physics::Component::ABABCollision3D>();

    EXPECT_EQ(view.size(), 0);
}
