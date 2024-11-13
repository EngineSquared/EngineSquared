#include <gtest/gtest.h>

#include "Entity.hpp"
#include "ParticleBoxCollision.hpp"
#include "Physics.hpp"
#include "RealTimeProvider.hpp"
#include "RealTimeUpdater.hpp"
#include "Registry.hpp"
#include "SoftBodyNode.hpp"
#include "Transform.hpp"
#include "VelocityIntegration.hpp"

#include "utils/Sleep.hpp"

TEST(SoftBodyCollisions, BasicParticleCollision)
{
    ES::Engine::Registry registry;

    registry.RegisterSystem(ES::Plugin::Collision::System::RemoveParticleBoxCollisions);
    registry.RegisterSystem(ES::Plugin::Collision::System::SoftBodyCollision);
    registry.RegisterResource<ES::Plugin::Time::Resource::RealTimeProvider>(
        ES::Plugin::Time::Resource::RealTimeProvider());
    registry.RegisterSystem(ES::Plugin::Physics::System::VelocityIntegration);
    registry.RegisterSystem(ES::Plugin::Time::System::RealTimeUpdater);

    ES::Engine::Entity particle = registry.CreateEntity();
    registry.GetRegistry().emplace<ES::Plugin::Object::Component::Transform>(particle, glm::vec3(0, 1, 0));
    registry.GetRegistry().emplace<ES::Plugin::Physics::Component::SoftBodyNode>(particle);

    ES::Engine::Entity ground = registry.CreateEntity();
    registry.GetRegistry().emplace<ES::Plugin::Object::Component::Transform>(ground, glm::vec3(0, 0, 0));
    registry.GetRegistry().emplace<ES::Plugin::Collision::Component::BoxCollider3D>(ground, glm::vec3(2, 2, 2));

    auto &node = registry.GetRegistry().get<ES::Plugin::Physics::Component::SoftBodyNode>(particle);
    auto &transform = registry.GetRegistry().get<ES::Plugin::Object::Component::Transform>(particle);

    registry.RunSystems();

    auto view = registry.GetRegistry().view<ES::Plugin::Collision::Component::ParticleBoxCollision>();

    ASSERT_GT(view.size(), 0);

    registry.GetRegistry().emplace<ES::Plugin::Object::Component::Transform>(particle, glm::vec3(0, 100, 0));

    registry.RunSystems();

    view = registry.GetRegistry().view<ES::Plugin::Collision::Component::ParticleBoxCollision>();

    ASSERT_EQ(view.size(), 0);
}

TEST(SoftBodyCollisions, VelocityIntegrationWithBasicCollision)
{
    ES::Engine::Registry registry;

    registry.RegisterSystem(ES::Plugin::Collision::System::SoftBodyCollision);
    registry.RegisterResource<ES::Plugin::Time::Resource::RealTimeProvider>(
        ES::Plugin::Time::Resource::RealTimeProvider());
    registry.RegisterSystem(ES::Plugin::Physics::System::VelocityIntegration);
    registry.RegisterSystem(ES::Plugin::Time::System::RealTimeUpdater);

    ES::Engine::Entity particle = registry.CreateEntity();
    registry.GetRegistry().emplace<ES::Plugin::Object::Component::Transform>(particle, glm::vec3(0, 2.1, 0));
    registry.GetRegistry().emplace<ES::Plugin::Physics::Component::SoftBodyNode>(particle);

    ES::Engine::Entity ground = registry.CreateEntity();
    registry.GetRegistry().emplace<ES::Plugin::Object::Component::Transform>(ground, glm::vec3(0, 0, 0));
    registry.GetRegistry().emplace<ES::Plugin::Collision::Component::BoxCollider3D>(ground, glm::vec3(2, 2, 2));

    auto &node = registry.GetRegistry().get<ES::Plugin::Physics::Component::SoftBodyNode>(particle);
    auto &transform = registry.GetRegistry().get<ES::Plugin::Object::Component::Transform>(particle);

    int i = 0;
    bool collisionHappened = false;

    while (!collisionHappened && i < 50)
    {
        SleepFor(10);
        registry.RunSystems();
        auto view = registry.GetRegistry().view<ES::Plugin::Collision::Component::ParticleBoxCollision>();
        if (view.size() > 0)
        {
            collisionHappened = true;
            break;
        }
        i++;
    }
    if (!collisionHappened)
    {
        FAIL() << "No collision happened";
    }

    for (i = 0; i < 100; i++)
    {
        SleepFor(10);
        registry.RunSystems();
    }

    // particle should have bounced and be on top of the box, so y should be positive
    EXPECT_GT(transform.position.y, 0);

    // force should be zero though
    EXPECT_EQ(node.force.y, 0);
}
