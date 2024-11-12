#include "Registry.hpp"

// Require object plugin components
#include "Transform.hpp"

#include "BoxCollider3D.hpp"
#include "CollisionStorage.hpp"

namespace ES::Plugin::Collision::System {
static bool isColliding(const glm::vec3 &posA, const ES::Plugin::Collision::Component::BoxCollider3D &boxA,
                        const glm::vec3 &posB, const ES::Plugin::Collision::Component::BoxCollider3D &boxB)
{
    glm::vec3 boundingBoxAMin = posA;
    glm::vec3 boundingBoxAMax = posA + boxA.size;
    glm::vec3 boundingBoxBMin = posB;
    glm::vec3 boundingBoxBMax = posB + boxB.size;

    return boundingBoxAMax.x >= boundingBoxBMin.x && boundingBoxAMax.y >= boundingBoxBMin.y &&
           boundingBoxAMax.z >= boundingBoxBMin.z && boundingBoxAMin.x <= boundingBoxBMax.x &&
           boundingBoxAMin.y <= boundingBoxBMax.y && boundingBoxAMin.z <= boundingBoxBMax.z;
}

void CollisionChecker(ES::Engine::Registry &registry)
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

            if (isColliding(transformA.position, boxColliderA, transformB.position, boxColliderB))
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
