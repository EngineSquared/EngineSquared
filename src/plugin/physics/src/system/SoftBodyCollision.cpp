#include "SoftBodyCollision.hpp"

#include "BoxCollider3D.hpp"
#include "Entity.hpp"
#include "ParticleBoxCollision.hpp"
#include "RealTimeProvider.hpp"
#include "SoftBodyNode.hpp"
#include "Transform.hpp"

static bool IsNodeInsideBox(const ES::Plugin::Object::Component::Transform &nodeTransform,
                            const ES::Plugin::Object::Component::Transform &boxTransform,
                            const ES::Plugin::Physics::Component::BoxCollider3D &boxCollider)
{
    return nodeTransform.position.x >= boxTransform.position.x - boxCollider.size.x / 2 &&
           nodeTransform.position.x <= boxTransform.position.x + boxCollider.size.x / 2 &&
           nodeTransform.position.y >= boxTransform.position.y - boxCollider.size.y / 2 &&
           nodeTransform.position.y <= boxTransform.position.y + boxCollider.size.y / 2 &&
           nodeTransform.position.z >= boxTransform.position.z - boxCollider.size.z / 2 &&
           nodeTransform.position.z <= boxTransform.position.z + boxCollider.size.z / 2;
}

void ES::Plugin::Physics::System::DetectSoftBodyCollisions(ES::Engine::Registry &registry)
{
    float dt = registry.GetResource<ES::Plugin::Time::Resource::RealTimeProvider>().GetElapsedTime();
    auto boxColliderView =
        registry.GetRegistry()
            .view<ES::Plugin::Physics::Component::BoxCollider3D, ES::Plugin::Object::Component::Transform>();
    auto nodeView = registry.GetRegistry()
                        .view<ES::Plugin::Physics::Component::SoftBodyNode, ES::Plugin::Object::Component::Transform>();

    for (auto boxEntity : boxColliderView)
    {
        auto &boxCollider = boxColliderView.get<ES::Plugin::Physics::Component::BoxCollider3D>(boxEntity);
        auto &boxTransform = boxColliderView.get<ES::Plugin::Object::Component::Transform>(boxEntity);

        for (auto nodeEntity : nodeView)
        {
            auto &node = nodeView.get<ES::Plugin::Physics::Component::SoftBodyNode>(nodeEntity);
            auto &nodeTransform = nodeView.get<ES::Plugin::Object::Component::Transform>(nodeEntity);

            if (IsNodeInsideBox(nodeTransform, boxTransform, boxCollider))
            {
                glm::vec3 nodeToBox = nodeTransform.position - boxTransform.position;

                float distances[3] = {
                    std::abs(nodeToBox.x) - boxCollider.size.x / 2,
                    std::abs(nodeToBox.y) - boxCollider.size.y / 2,
                    std::abs(nodeToBox.z) - boxCollider.size.z / 2,
                };

                int closestAxis = 0;
                for (int i = 1; i < 3; ++i)
                {
                    if (distances[i] > distances[closestAxis])
                    {
                        closestAxis = i;
                    }
                }

                glm::vec3 boxNormal(0.0f);
                if (closestAxis == 0)
                    boxNormal.x = (nodeToBox.x > 0) ? 1.0f : -1.0f;
                else if (closestAxis == 1)
                    boxNormal.y = (nodeToBox.y > 0) ? 1.0f : -1.0f;
                else if (closestAxis == 2)
                    boxNormal.z = (nodeToBox.z > 0) ? 1.0f : -1.0f;

                float depth = std::abs(distances[closestAxis]);

                ES::Engine::Entity collision = registry.CreateEntity();
                collision.AddComponent<ES::Plugin::Physics::Component::ParticleBoxCollision>(
                    registry, nodeEntity, boxEntity, boxNormal, depth);
            }
        }
    }
}

void ES::Plugin::Physics::System::ApplySoftBodyCollisions(ES::Engine::Registry &registry)
{
    auto dt = registry.GetFixedDeltaTime();
    auto nodeView = registry.GetRegistry().view<ES::Plugin::Physics::Component::ParticleBoxCollision>();

    for (auto entity : nodeView)
    {
        auto &collision = nodeView.get<ES::Plugin::Physics::Component::ParticleBoxCollision>(entity);
        auto &node = registry.GetRegistry().get<ES::Plugin::Physics::Component::SoftBodyNode>(collision.particleEntity);
        auto &nodeTransform =
            registry.GetRegistry().get<ES::Plugin::Object::Component::Transform>(collision.particleEntity);

        nodeTransform.position += collision.penetrationDepth * collision.normal;

        glm::vec3 vn = glm::dot(collision.normal, node.velocity) * collision.normal;
        glm::vec3 vt = node.velocity - vn;

        vn *= -node.elasticity;
        vt *= std::exp(-node.friction * dt);

        node.velocity = vn + vt;

        registry.GetRegistry().destroy(entity);
    }
}

void ES::Plugin::Physics::System::DeleteSoftBodyCollisions(ES::Engine::Registry &registry)
{
    auto nodeView = registry.GetRegistry().view<ES::Plugin::Physics::Component::ParticleBoxCollision>();

    for (auto entity : nodeView)
    {
        registry.GetRegistry().destroy(entity);
    }
}
