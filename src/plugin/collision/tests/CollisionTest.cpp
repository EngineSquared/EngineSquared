#include <gtest/gtest.h>

#include "BoxCollider3D.hpp"
#include "CollisionChecker.hpp"
#include "CollisionStorage.hpp"
#include "Transform.hpp"

using namespace ES::Plugin::Collision;

TEST(Collision, CollisionSystemWithBoxCollider3D)
{
    ES::Engine::Registry registry;
    registry.RegisterResource<Resource::CollisionStorage>(Resource::CollisionStorage());

    ES::Engine::Entity eA(registry.CreateEntity());
    ES::Engine::Entity eB(registry.CreateEntity());

    eA.AddComponent<ES::Plugin::Collision::Component::BoxCollider3D>(registry, glm::vec3(1, 1, 1));
    eB.AddComponent<ES::Plugin::Collision::Component::BoxCollider3D>(registry, glm::vec3(1, 1, 1));
    eA.AddComponent<ES::Plugin::Object::Component::Transform>(registry, glm::vec3(1, 1, 1));
    eB.AddComponent<ES::Plugin::Object::Component::Transform>(registry, glm::vec3(1, 1, 1));

    registry.RegisterSystem(ES::Plugin::Collision::System::ResetCollision);
    registry.RegisterSystem(ES::Plugin::Collision::System::CollisionChecker);

    registry.RunSystems();

    EXPECT_TRUE(registry.GetResource<Resource::CollisionStorage>().IsCollidingPair(eA, eB));

    eA.GetComponents<ES::Plugin::Object::Component::Transform>(registry).setPosition(9, 9, 9);

    registry.RunSystems();

    EXPECT_FALSE(registry.GetResource<Resource::CollisionStorage>().IsCollidingPair(eA, eB));
}
