#include "Registry.hpp"

// Require object plugin components
#include "Transform.hpp"

#include "ABABCollision3D.hpp"
#include "BoxCollider3D.hpp"
#include "CollisionCheckerABAB.hpp"
#include "CollisionUtils3D.hpp"

void ES::Plugin::Physics::System::DetectABABCollisions(ES::Engine::Registry &registry)
{
    auto view = registry.GetRegistry()
                    .view<const ES::Plugin::Object::Component::Transform,
                          const ES::Plugin::Physics::Component::BoxCollider3D>();

    for (auto entityA : view)
    {
        for (auto entityB : view)
        {
            if (entityA <= entityB)
                continue;

            const auto &transformA = view.get<const ES::Plugin::Object::Component::Transform>(entityA);
            const auto &transformB = view.get<const ES::Plugin::Object::Component::Transform>(entityB);
            const auto &boxColliderA = view.get<const ES::Plugin::Physics::Component::BoxCollider3D>(entityA);
            const auto &boxColliderB = view.get<const ES::Plugin::Physics::Component::BoxCollider3D>(entityB);

            if (ES::Plugin::Physics::Utils::Box3DCollidesBox3D(transformA.position, boxColliderA, transformB.position,
                                                               boxColliderB))
            {
                ES::Engine::Entity collision = registry.CreateEntity();
                collision.AddComponent<ES::Plugin::Physics::Component::ABABCollision3D>(registry, entityA, entityB);
            }
        }
    }
}

void ES::Plugin::Physics::System::RemoveABABCollisions(ES::Engine::Registry &registry)
{
    auto view = registry.GetRegistry().view<ES::Plugin::Physics::Component::ABABCollision3D>();

    for (auto entity : view)
    {
        registry.GetRegistry().destroy(entity);
    }
}
