#include "Registry.hpp"

// Require object plugin components
#include "Transform.hpp"

#include "3DCollisions.hpp"
#include "BoxCollider3D.hpp"
#include "CollisionStorage.hpp"

namespace ES::Plugin::Collision::System {
void CollisionChecker3D(ES::Engine::Registry &registry)
{
    auto view = registry.GetRegistry()
                    .view<const ES::Plugin::Object::Component::Transform,
                          const ES::Plugin::Collision::Component::BoxCollider3D>();

    for (auto entityA : view)
    {
        for (auto entityB : view)
        {
            if (entityA <= entityB)
                continue;

            const auto &transformA = view.get<const ES::Plugin::Object::Component::Transform>(entityA);
            const auto &transformB = view.get<const ES::Plugin::Object::Component::Transform>(entityB);
            const auto &boxColliderA = view.get<const ES::Plugin::Collision::Component::BoxCollider3D>(entityA);
            const auto &boxColliderB = view.get<const ES::Plugin::Collision::Component::BoxCollider3D>(entityB);

            if (Utils::Box3DCollidesBox3D(transformA.position, boxColliderA, transformB.position, boxColliderB))
            {
                registry.GetResource<ES::Plugin::Collision::Resource::CollisionStorage>().AddCollisionPair(
                    ES::Engine::Entity::FromEnttEntity(entityA), ES::Engine::Entity::FromEnttEntity(entityB));
            }
        }
    }
}

void ResetCollision(ES::Engine::Registry &registry)
{
    registry.GetResource<ES::Plugin::Collision::Resource::CollisionStorage>().ClearAllCollisionPair();
}
} // namespace ES::Plugin::Collision::System
