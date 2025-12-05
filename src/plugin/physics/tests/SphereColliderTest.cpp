#include <gtest/gtest.h>

#include "Engine.hpp"
#include "Object.hpp"
#include "Physics.hpp"

using namespace Physics::Component;
using namespace Physics::Resource;

//=============================================================================
// Unit Tests - SphereCollider Component
//=============================================================================

TEST(SphereColliderTest, DefaultConstruction)
{
    SphereCollider collider;

    EXPECT_FLOAT_EQ(collider.radius, 0.5f);
    EXPECT_EQ(collider.offset, glm::vec3(0.0f));
    EXPECT_TRUE(collider.IsValid());
}

TEST(SphereColliderTest, ConstructWithRadius)
{
    SphereCollider collider(1.5f);

    EXPECT_FLOAT_EQ(collider.radius, 1.5f);
    EXPECT_EQ(collider.offset, glm::vec3(0.0f));
}

TEST(SphereColliderTest, ConstructWithRadiusAndOffset)
{
    SphereCollider collider(2.0f, glm::vec3(1.0f, 2.0f, 3.0f));

    EXPECT_FLOAT_EQ(collider.radius, 2.0f);
    EXPECT_EQ(collider.offset, glm::vec3(1.0f, 2.0f, 3.0f));
}

TEST(SphereColliderTest, GetDiameter)
{
    SphereCollider collider(0.75f);

    EXPECT_FLOAT_EQ(collider.GetDiameter(), 1.5f);
}

TEST(SphereColliderTest, SetDiameter)
{
    SphereCollider collider;
    collider.SetDiameter(3.0f);

    EXPECT_FLOAT_EQ(collider.radius, 1.5f);
}

TEST(SphereColliderTest, IsValidWithPositiveRadius)
{
    SphereCollider collider(0.001f);
    EXPECT_TRUE(collider.IsValid());
}

TEST(SphereColliderTest, IsValidWithZeroRadius)
{
    SphereCollider collider;
    collider.radius = 0.0f;
    EXPECT_FALSE(collider.IsValid());
}

TEST(SphereColliderTest, IsValidWithNegativeRadius)
{
    SphereCollider collider;
    collider.radius = -1.0f;
    EXPECT_FALSE(collider.IsValid());
}

TEST(SphereColliderTest, FactoryUnit)
{
    auto collider = SphereCollider::Unit();

    EXPECT_FLOAT_EQ(collider.radius, 1.0f);
}

TEST(SphereColliderTest, FactoryBall)
{
    auto collider = SphereCollider::Ball(0.11f); // Tennis ball ~11cm diameter

    EXPECT_FLOAT_EQ(collider.radius, 0.11f);
}

TEST(SphereColliderTest, FactoryAtOffset)
{
    auto collider = SphereCollider::AtOffset(0.5f, glm::vec3(0.0f, 1.0f, 0.0f));

    EXPECT_FLOAT_EQ(collider.radius, 0.5f);
    EXPECT_EQ(collider.offset, glm::vec3(0.0f, 1.0f, 0.0f));
}

//=============================================================================
// Integration Tests - SphereCollider with Physics System
//=============================================================================

class SphereColliderIntegrationTest : public ::testing::Test {
  protected:
    void SetUp() override
    {
        core = std::make_unique<Engine::Core>();
        core->AddPlugins<Physics::Plugin>();
        core->RunSystems();
    }

    void TearDown() override { core.reset(); }

    std::unique_ptr<Engine::Core> core;
};

TEST_F(SphereColliderIntegrationTest, CreateDynamicBodyWithSphereCollider)
{
    auto &registry = core->GetRegistry();

    // Create entity with Transform
    Engine::Entity entity(core->CreateEntity());
    Object::Component::Transform transform;
    transform.SetPosition(glm::vec3(0.0f, 5.0f, 0.0f));
    registry.emplace<Object::Component::Transform>(entity, transform);

    // Add SphereCollider
    SphereCollider sphereCollider(0.5f);
    registry.emplace<SphereCollider>(entity, sphereCollider);

    // Add dynamic RigidBody
    RigidBody rb = RigidBody::CreateDynamic(1.0f);
    registry.emplace<RigidBody>(entity, rb);

    // Verify RigidBodyInternal was created
    auto *internal = registry.try_get<RigidBodyInternal>(entity);
    ASSERT_NE(internal, nullptr);
    EXPECT_TRUE(internal->IsValid());

    // Verify NO DefaultCollider was created (SphereCollider should prevent auto-creation)
    auto *defaultCollider = registry.try_get<DefaultCollider>(entity);
    EXPECT_EQ(defaultCollider, nullptr);
}

TEST_F(SphereColliderIntegrationTest, CreateStaticBodyWithSphereCollider)
{
    auto &registry = core->GetRegistry();

    Engine::Entity entity(core->CreateEntity());
    Object::Component::Transform transform;
    transform.SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    registry.emplace<Object::Component::Transform>(entity, transform);

    SphereCollider sphereCollider(10.0f); // Large floor sphere
    registry.emplace<SphereCollider>(entity, sphereCollider);

    RigidBody rb = RigidBody::CreateStatic();
    registry.emplace<RigidBody>(entity, rb);

    auto *internal = registry.try_get<RigidBodyInternal>(entity);
    ASSERT_NE(internal, nullptr);
    EXPECT_TRUE(internal->IsValid());
}

TEST_F(SphereColliderIntegrationTest, SphereColliderPriorityOverBoxCollider)
{
    auto &registry = core->GetRegistry();

    Engine::Entity entity(core->CreateEntity());
    Object::Component::Transform transform;
    registry.emplace<Object::Component::Transform>(entity, transform);

    // Add BOTH colliders - SphereCollider should have higher priority
    BoxCollider boxCollider(glm::vec3(1.0f));
    registry.emplace<BoxCollider>(entity, boxCollider);

    SphereCollider sphereCollider(0.5f);
    registry.emplace<SphereCollider>(entity, sphereCollider);

    RigidBody rb = RigidBody::CreateDynamic(1.0f);
    registry.emplace<RigidBody>(entity, rb);

    // RigidBody should be created successfully
    auto *internal = registry.try_get<RigidBodyInternal>(entity);
    ASSERT_NE(internal, nullptr);
    EXPECT_TRUE(internal->IsValid());

    // No DefaultCollider should be created
    auto *defaultCollider = registry.try_get<DefaultCollider>(entity);
    EXPECT_EQ(defaultCollider, nullptr);
}

TEST_F(SphereColliderIntegrationTest, MultipleSphereEntities)
{
    auto &registry = core->GetRegistry();

    // Create multiple sphere entities
    for (int i = 0; i < 5; ++i)
    {
        Engine::Entity entity(core->CreateEntity());
        Object::Component::Transform transform;
        transform.SetPosition(glm::vec3(static_cast<float>(i) * 2.0f, 5.0f, 0.0f));
        registry.emplace<Object::Component::Transform>(entity, transform);

        SphereCollider sphereCollider(0.5f);
        registry.emplace<SphereCollider>(entity, sphereCollider);

        RigidBody rb = RigidBody::CreateDynamic(1.0f);
        registry.emplace<RigidBody>(entity, rb);
    }

    // Count entities with RigidBodyInternal
    int count = 0;
    registry.view<RigidBodyInternal>().each([&count](auto &) { ++count; });

    EXPECT_EQ(count, 5);
}
