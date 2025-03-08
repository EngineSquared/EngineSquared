#include <gtest/gtest.h>

#include "Core.hpp"
#include "Entity.hpp"
#include "ParticleBoxCollision.hpp"
#include "Physics.hpp"
#include "SoftBodyNode.hpp"
#include "Transform.hpp"
#include "VelocityIntegration.hpp"

#include "utils/Sleep.hpp"

TEST(SoftBodyCollisions, BasicParticleCollision)
{
    ES::Engine::Core core;

    core.RegisterSystem(ES::Plugin::Physics::System::DetectSoftBodyCollisions);

    ES::Engine::Entity particle = core.CreateEntity();
    core.GetRegistry().emplace<ES::Plugin::Object::Component::Transform>(particle, glm::vec3(0, 1, 0));
    core.GetRegistry().emplace<ES::Plugin::Physics::Component::SoftBodyNode>(particle);

    ES::Engine::Entity ground = core.CreateEntity();
    core.GetRegistry().emplace<ES::Plugin::Object::Component::Transform>(ground, glm::vec3(0, 0, 0));
    core.GetRegistry().emplace<ES::Plugin::Physics::Component::BoxCollider3D>(ground, glm::vec3(2, 2, 2));

    core.RunSystems();

    auto view = core.GetRegistry().view<ES::Plugin::Physics::Component::ParticleBoxCollision>();

    ASSERT_GT(view.size(), 0);

    for (auto &entity : view)
    {
        core.GetRegistry().destroy(entity);
    }

    core.GetRegistry().emplace<ES::Plugin::Object::Component::Transform>(particle, glm::vec3(0, 100, 0));

    core.RunSystems();

    view = core.GetRegistry().view<ES::Plugin::Physics::Component::ParticleBoxCollision>();

    ASSERT_EQ(view.size(), 0);
}

TEST(SoftBodyCollisions, VelocityIntegrationWithBasicCollision)
{
    ES::Engine::Core core;

    core.RegisterSystem(ES::Plugin::Physics::System::DetectSoftBodyCollisions);
    core.RegisterSystem(ES::Plugin::Physics::System::VelocityIntegration);
    core.RegisterSystem(ES::Plugin::Physics::System::ApplySoftBodyCollisions);

    ES::Engine::Entity particle = core.CreateEntity();
    core.GetRegistry().emplace<ES::Plugin::Object::Component::Transform>(particle, glm::vec3(0, 2.1, 0));
    core.GetRegistry().emplace<ES::Plugin::Physics::Component::SoftBodyNode>(particle);

    ES::Engine::Entity ground = core.CreateEntity();
    core.GetRegistry().emplace<ES::Plugin::Object::Component::Transform>(ground, glm::vec3(0, 0, 0));
    core.GetRegistry().emplace<ES::Plugin::Physics::Component::BoxCollider3D>(ground, glm::vec3(2, 2, 2));

    auto const &node = core.GetRegistry().get<ES::Plugin::Physics::Component::SoftBodyNode>(particle);
    auto const &transform = core.GetRegistry().get<ES::Plugin::Object::Component::Transform>(particle);

    bool bounced = false;

    for (int i = 0; i < 100; i++)
    {
        SleepFor(10);
        core.RunSystems();

        if (node.velocity.y > 0)
        {
            bounced = true;
        }
        // Prevent gravity from making the particle bounce again
        if (bounced && node.velocity.y <= 0)
        {
            break;
        }
    }

    if (!bounced)
    {
        FAIL() << "Particle did not bounce";
    }

    // particle should have bounced and be on top of the box, so y should be higher than 1
    // (because the box is at 0,0,0 and has a size of 2)
    EXPECT_GT(transform.position.y, 1);

    // force should be zero though
    EXPECT_EQ(node.force.y, 0);
}
