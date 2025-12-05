#include <gtest/gtest.h>

#include "Engine.hpp"
#include "Object.hpp"
#include "Physics.hpp"

using namespace Physics::Component;
using namespace Physics::Resource;

//=============================================================================
// Unit Tests - CapsuleCollider Component
//=============================================================================

TEST(CapsuleColliderTest, DefaultConstruction)
{
    CapsuleCollider collider;

    EXPECT_FLOAT_EQ(collider.halfHeight, 0.5f);
    EXPECT_FLOAT_EQ(collider.radius, 0.25f);
    EXPECT_EQ(collider.offset, glm::vec3(0.0f));
    EXPECT_TRUE(collider.IsValid());
}

TEST(CapsuleColliderTest, ConstructWithDimensions)
{
    CapsuleCollider collider(0.8f, 0.3f);

    EXPECT_FLOAT_EQ(collider.halfHeight, 0.8f);
    EXPECT_FLOAT_EQ(collider.radius, 0.3f);
}

TEST(CapsuleColliderTest, ConstructWithDimensionsAndOffset)
{
    CapsuleCollider collider(0.5f, 0.25f, glm::vec3(0.0f, 1.0f, 0.0f));

    EXPECT_FLOAT_EQ(collider.halfHeight, 0.5f);
    EXPECT_FLOAT_EQ(collider.radius, 0.25f);
    EXPECT_EQ(collider.offset, glm::vec3(0.0f, 1.0f, 0.0f));
}

TEST(CapsuleColliderTest, GetTotalHeight)
{
    // halfHeight=0.5, radius=0.25 => total = (0.5+0.25)*2 = 1.5
    CapsuleCollider collider(0.5f, 0.25f);

    EXPECT_FLOAT_EQ(collider.GetTotalHeight(), 1.5f);
}

TEST(CapsuleColliderTest, GetCylinderHeight)
{
    CapsuleCollider collider(0.8f, 0.3f);

    EXPECT_FLOAT_EQ(collider.GetCylinderHeight(), 1.6f);  // 0.8 * 2
}

TEST(CapsuleColliderTest, GetDiameter)
{
    CapsuleCollider collider(0.5f, 0.4f);

    EXPECT_FLOAT_EQ(collider.GetDiameter(), 0.8f);  // 0.4 * 2
}

TEST(CapsuleColliderTest, SetTotalHeight)
{
    CapsuleCollider collider(0.5f, 0.25f);  // Original halfHeight=0.5, radius=0.25
    collider.SetTotalHeight(2.0f);

    // totalHeight = (halfHeight + radius) * 2
    // 2.0 = (halfHeight + 0.25) * 2
    // 1.0 = halfHeight + 0.25
    // halfHeight = 0.75
    EXPECT_FLOAT_EQ(collider.halfHeight, 0.75f);
    EXPECT_FLOAT_EQ(collider.radius, 0.25f);  // Radius unchanged
}

TEST(CapsuleColliderTest, IsValidWithPositiveDimensions)
{
    CapsuleCollider collider(0.5f, 0.25f);
    EXPECT_TRUE(collider.IsValid());
}

TEST(CapsuleColliderTest, IsValidWithZeroHalfHeight)
{
    CapsuleCollider collider(0.0f, 0.25f);  // Valid - degenerates to sphere
    EXPECT_TRUE(collider.IsValid());
}

TEST(CapsuleColliderTest, IsValidWithNegativeHalfHeight)
{
    CapsuleCollider collider;
    collider.halfHeight = -0.5f;
    EXPECT_FALSE(collider.IsValid());
}

TEST(CapsuleColliderTest, IsValidWithZeroRadius)
{
    CapsuleCollider collider;
    collider.radius = 0.0f;
    EXPECT_FALSE(collider.IsValid());
}

TEST(CapsuleColliderTest, IsSphereWhenHalfHeightZero)
{
    CapsuleCollider collider(0.0f, 0.5f);
    EXPECT_TRUE(collider.IsSphere());
}

TEST(CapsuleColliderTest, NotSphereWhenHalfHeightPositive)
{
    CapsuleCollider collider(0.1f, 0.5f);
    EXPECT_FALSE(collider.IsSphere());
}

TEST(CapsuleColliderTest, FactoryForCharacter)
{
    // 1.8m tall character, 0.3m radius
    auto collider = CapsuleCollider::ForCharacter(1.8f, 0.3f);

    // halfHeight = (1.8 / 2) - 0.3 = 0.9 - 0.3 = 0.6
    EXPECT_FLOAT_EQ(collider.halfHeight, 0.6f);
    EXPECT_FLOAT_EQ(collider.radius, 0.3f);
    EXPECT_FLOAT_EQ(collider.GetTotalHeight(), 1.8f);
}

TEST(CapsuleColliderTest, FactoryFromTotalHeight)
{
    auto collider = CapsuleCollider::FromTotalHeight(2.0f, 0.4f);

    // halfHeight = (2.0 / 2) - 0.4 = 1.0 - 0.4 = 0.6
    EXPECT_FLOAT_EQ(collider.halfHeight, 0.6f);
    EXPECT_FLOAT_EQ(collider.radius, 0.4f);
}

TEST(CapsuleColliderTest, FactoryFromTotalHeightDegeneratesToSphere)
{
    // If totalHeight <= 2*radius, halfHeight would be negative or zero
    auto collider = CapsuleCollider::FromTotalHeight(0.5f, 0.4f);

    // halfHeight = (0.5 / 2) - 0.4 = 0.25 - 0.4 = -0.15 -> clamped to 0
    EXPECT_FLOAT_EQ(collider.halfHeight, 0.0f);
    EXPECT_TRUE(collider.IsSphere());
}

TEST(CapsuleColliderTest, FactoryHorizontal)
{
    auto collider = CapsuleCollider::Horizontal(3.0f, 0.5f);

    EXPECT_FLOAT_EQ(collider.GetTotalHeight(), 3.0f);
    EXPECT_FLOAT_EQ(collider.radius, 0.5f);
}

TEST(CapsuleColliderTest, FactoryAtOffset)
{
    auto collider = CapsuleCollider::AtOffset(0.5f, 0.25f, glm::vec3(0.0f, 0.5f, 0.0f));

    EXPECT_FLOAT_EQ(collider.halfHeight, 0.5f);
    EXPECT_FLOAT_EQ(collider.radius, 0.25f);
    EXPECT_EQ(collider.offset, glm::vec3(0.0f, 0.5f, 0.0f));
}

//=============================================================================
// Integration Tests - CapsuleCollider with Physics System
//=============================================================================

class CapsuleColliderIntegrationTest : public ::testing::Test {
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

TEST_F(CapsuleColliderIntegrationTest, CreateDynamicBodyWithCapsuleCollider)
{
    auto &registry = core->GetRegistry();

    Engine::Entity entity(core->CreateEntity());
    Object::Component::Transform transform;
    transform.SetPosition(glm::vec3(0.0f, 5.0f, 0.0f));
    registry.emplace<Object::Component::Transform>(entity, transform);

    // Character-sized capsule
    auto capsuleCollider = CapsuleCollider::ForCharacter(1.8f, 0.3f);
    registry.emplace<CapsuleCollider>(entity, capsuleCollider);

    RigidBody rb = RigidBody::CreateDynamic(70.0f);  // 70kg character
    registry.emplace<RigidBody>(entity, rb);

    auto *internal = registry.try_get<RigidBodyInternal>(entity);
    ASSERT_NE(internal, nullptr);
    EXPECT_TRUE(internal->IsValid());

    // No DefaultCollider should be created
    auto *defaultCollider = registry.try_get<DefaultCollider>(entity);
    EXPECT_EQ(defaultCollider, nullptr);
}

TEST_F(CapsuleColliderIntegrationTest, CreateKinematicBodyWithCapsuleCollider)
{
    auto &registry = core->GetRegistry();

    Engine::Entity entity(core->CreateEntity());
    Object::Component::Transform transform;
    registry.emplace<Object::Component::Transform>(entity, transform);

    CapsuleCollider capsuleCollider(0.5f, 0.25f);
    registry.emplace<CapsuleCollider>(entity, capsuleCollider);

    RigidBody rb = RigidBody::CreateKinematic();
    registry.emplace<RigidBody>(entity, rb);

    auto *internal = registry.try_get<RigidBodyInternal>(entity);
    ASSERT_NE(internal, nullptr);
    EXPECT_TRUE(internal->IsValid());
}

TEST_F(CapsuleColliderIntegrationTest, CapsuleColliderPriorityOverBoxCollider)
{
    auto &registry = core->GetRegistry();

    Engine::Entity entity(core->CreateEntity());
    Object::Component::Transform transform;
    registry.emplace<Object::Component::Transform>(entity, transform);

    // Add BoxCollider first
    BoxCollider boxCollider(glm::vec3(1.0f));
    registry.emplace<BoxCollider>(entity, boxCollider);

    // Then add CapsuleCollider - should have higher priority than Box
    CapsuleCollider capsuleCollider(0.5f, 0.25f);
    registry.emplace<CapsuleCollider>(entity, capsuleCollider);

    RigidBody rb = RigidBody::CreateDynamic(1.0f);
    registry.emplace<RigidBody>(entity, rb);

    auto *internal = registry.try_get<RigidBodyInternal>(entity);
    ASSERT_NE(internal, nullptr);
    EXPECT_TRUE(internal->IsValid());
}

TEST_F(CapsuleColliderIntegrationTest, SphereColliderPriorityOverCapsuleCollider)
{
    auto &registry = core->GetRegistry();

    Engine::Entity entity(core->CreateEntity());
    Object::Component::Transform transform;
    registry.emplace<Object::Component::Transform>(entity, transform);

    // Add CapsuleCollider first
    CapsuleCollider capsuleCollider(0.5f, 0.25f);
    registry.emplace<CapsuleCollider>(entity, capsuleCollider);

    // Then add SphereCollider - should have highest priority
    SphereCollider sphereCollider(0.5f);
    registry.emplace<SphereCollider>(entity, sphereCollider);

    RigidBody rb = RigidBody::CreateDynamic(1.0f);
    registry.emplace<RigidBody>(entity, rb);

    auto *internal = registry.try_get<RigidBodyInternal>(entity);
    ASSERT_NE(internal, nullptr);
    EXPECT_TRUE(internal->IsValid());
}

TEST_F(CapsuleColliderIntegrationTest, MultipleCapsuleEntities)
{
    auto &registry = core->GetRegistry();

    // Create multiple capsule entities (simulating characters)
    for (int i = 0; i < 3; ++i)
    {
        Engine::Entity entity(core->CreateEntity());
        Object::Component::Transform transform;
        transform.SetPosition(glm::vec3(static_cast<float>(i) * 2.0f, 0.0f, 0.0f));
        registry.emplace<Object::Component::Transform>(entity, transform);

        auto capsuleCollider = CapsuleCollider::ForCharacter(1.8f, 0.3f);
        registry.emplace<CapsuleCollider>(entity, capsuleCollider);

        RigidBody rb = RigidBody::CreateDynamic(70.0f);
        registry.emplace<RigidBody>(entity, rb);
    }

    int count = 0;
    registry.view<RigidBodyInternal>().each([&count](auto &) { ++count; });

    EXPECT_EQ(count, 3);
}
