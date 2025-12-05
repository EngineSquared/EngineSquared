#include <gtest/gtest.h>

#include "Engine.hpp"
#include "Object.hpp"
#include "Physics.hpp"

using namespace Physics::Component;
using namespace Physics::Resource;

//=============================================================================
// Test Fixture
//=============================================================================

class BasicConstraintsTest : public ::testing::Test {
  protected:
    void SetUp() override
    {
        core = std::make_unique<Engine::Core>();
        core->AddPlugins<Physics::Plugin>();
        core->RunSystems();

        // ConstraintSystem is now initialized automatically by PhysicsPlugin

        // Create body A (static anchor)
        entityA = Engine::Entity(core->CreateEntity());
        Object::Component::Transform transformA;
        transformA.SetPosition(glm::vec3(0.0f, 5.0f, 0.0f));
        core->GetRegistry().emplace<Object::Component::Transform>(entityA, transformA);

        Physics::Component::RigidBody rbA = Physics::Component::RigidBody::CreateStatic();
        core->GetRegistry().emplace<RigidBody>(entityA, rbA);

        // Create body B (dynamic)
        entityB = Engine::Entity(core->CreateEntity());
        Object::Component::Transform transformB;
        transformB.SetPosition(glm::vec3(0.0f, 3.0f, 0.0f));
        core->GetRegistry().emplace<Object::Component::Transform>(entityB, transformB);

        Physics::Component::RigidBody rbB = Physics::Component::RigidBody::CreateDynamic(1.0f);
        core->GetRegistry().emplace<RigidBody>(entityB, rbB);

        internalA = core->GetRegistry().try_get<RigidBodyInternal>(entityA);
        internalB = core->GetRegistry().try_get<RigidBodyInternal>(entityB);
    }

    void TearDown() override { core.reset(); }

    std::unique_ptr<Engine::Core> core;
    Engine::Entity entityA{0};
    Engine::Entity entityB{0};
    RigidBodyInternal *internalA = nullptr;
    RigidBodyInternal *internalB = nullptr;
};

//=============================================================================
// ConstraintSettings Tests
//=============================================================================

TEST(ConstraintSettingsTest, RigidFactoryDefaults)
{
    auto settings = ConstraintSettings::Rigid();

    EXPECT_FLOAT_EQ(settings.stiffness, 1.0f);
    EXPECT_FLOAT_EQ(settings.damping, 0.0f);
    EXPECT_FLOAT_EQ(settings.breakForce, 0.0f);
    EXPECT_FLOAT_EQ(settings.breakTorque, 0.0f);
    EXPECT_FALSE(settings.enableCollision);
    EXPECT_TRUE(settings.IsRigid());
    EXPECT_FALSE(settings.IsBreakable());
}

TEST(ConstraintSettingsTest, SoftFactoryValues)
{
    auto settings = ConstraintSettings::Soft(0.5f, 0.3f);

    EXPECT_FLOAT_EQ(settings.stiffness, 0.5f);
    EXPECT_FLOAT_EQ(settings.damping, 0.3f);
    EXPECT_FLOAT_EQ(settings.breakForce, 0.0f);
    EXPECT_FLOAT_EQ(settings.breakTorque, 0.0f);
    EXPECT_FALSE(settings.IsRigid());
    EXPECT_FALSE(settings.IsBreakable());
}

TEST(ConstraintSettingsTest, BreakableFactoryValues)
{
    auto settings = ConstraintSettings::Breakable(100.0f, 50.0f);

    EXPECT_FLOAT_EQ(settings.stiffness, 1.0f);
    EXPECT_FLOAT_EQ(settings.damping, 0.0f);
    EXPECT_FLOAT_EQ(settings.breakForce, 100.0f);
    EXPECT_FLOAT_EQ(settings.breakTorque, 50.0f);
    EXPECT_TRUE(settings.IsRigid());
    EXPECT_TRUE(settings.IsBreakable());
}

TEST(ConstraintSettingsTest, BreakableForceOnly)
{
    auto settings = ConstraintSettings::Breakable(100.0f);

    EXPECT_FLOAT_EQ(settings.breakForce, 100.0f);
    EXPECT_FLOAT_EQ(settings.breakTorque, 0.0f);
    EXPECT_TRUE(settings.IsBreakable());  // Force alone makes it breakable
}

//=============================================================================
// FixedConstraint Component Tests
//=============================================================================

TEST(FixedConstraintComponentTest, CreateBetweenBodies)
{
    Engine::Entity mockA(entt::entity(1));
    Engine::Entity mockB(entt::entity(2));

    auto constraint = FixedConstraint::Create(
        mockA, mockB,
        glm::vec3(0.0f, -0.5f, 0.0f),
        glm::vec3(0.0f, 0.5f, 0.0f),
        ConstraintSettings::Rigid());

    EXPECT_EQ(constraint.bodyA, mockA);
    EXPECT_EQ(constraint.bodyB, mockB);
    EXPECT_EQ(constraint.localPointA, glm::vec3(0.0f, -0.5f, 0.0f));
    EXPECT_EQ(constraint.localPointB, glm::vec3(0.0f, 0.5f, 0.0f));
    EXPECT_FALSE(constraint.IsWorldConstraint());
    EXPECT_FALSE(constraint.broken);
}

TEST(FixedConstraintComponentTest, CreateToWorld)
{
    Engine::Entity mockBody(entt::entity(1));

    auto constraint = FixedConstraint::CreateToWorld(
        mockBody,
        glm::vec3(0.0f, 10.0f, 0.0f),
        ConstraintSettings::Rigid());

    EXPECT_EQ(constraint.bodyA, mockBody);
    EXPECT_FALSE(constraint.bodyB.IsValid());
    EXPECT_TRUE(constraint.IsWorldConstraint());
    EXPECT_EQ(constraint.localPointB, glm::vec3(0.0f, 10.0f, 0.0f));  // World position
}

//=============================================================================
// DistanceConstraint Component Tests
//=============================================================================

TEST(DistanceConstraintComponentTest, CreateFixedDistance)
{
    Engine::Entity mockA(entt::entity(1));
    Engine::Entity mockB(entt::entity(2));

    auto constraint = DistanceConstraint::Create(
        mockA, mockB,
        3.0f,  // Fixed distance
        glm::vec3(0.0f),
        glm::vec3(0.0f),
        ConstraintSettings::Rigid());

    EXPECT_EQ(constraint.bodyA, mockA);
    EXPECT_EQ(constraint.bodyB, mockB);
    EXPECT_FLOAT_EQ(constraint.minDistance, 3.0f);
    EXPECT_FLOAT_EQ(constraint.maxDistance, 3.0f);
    EXPECT_TRUE(constraint.IsFixedDistance());
    EXPECT_FALSE(constraint.IsWorldConstraint());
    EXPECT_FALSE(constraint.IsAutoDistance());
}

TEST(DistanceConstraintComponentTest, CreateWithRange)
{
    Engine::Entity mockA(entt::entity(1));
    Engine::Entity mockB(entt::entity(2));

    auto constraint = DistanceConstraint::CreateWithRange(
        mockA, mockB,
        1.0f, 5.0f,  // Min and max distance (spring)
        glm::vec3(0.0f),
        glm::vec3(0.0f),
        ConstraintSettings::Soft(0.5f, 0.1f));

    EXPECT_FLOAT_EQ(constraint.minDistance, 1.0f);
    EXPECT_FLOAT_EQ(constraint.maxDistance, 5.0f);
    EXPECT_FALSE(constraint.IsFixedDistance());
}

TEST(DistanceConstraintComponentTest, CreateAutoDistance)
{
    Engine::Entity mockA(entt::entity(1));
    Engine::Entity mockB(entt::entity(2));

    auto constraint = DistanceConstraint::CreateAutoDistance(
        mockA, mockB,
        glm::vec3(0.0f),
        glm::vec3(0.0f),
        ConstraintSettings::Rigid());

    EXPECT_FLOAT_EQ(constraint.minDistance, -1.0f);
    EXPECT_FLOAT_EQ(constraint.maxDistance, -1.0f);
    EXPECT_TRUE(constraint.IsAutoDistance());
}

TEST(DistanceConstraintComponentTest, CreateToWorld)
{
    Engine::Entity mockBody(entt::entity(1));

    auto constraint = DistanceConstraint::CreateToWorld(
        mockBody,
        glm::vec3(0.0f, 10.0f, 0.0f),
        3.0f,
        glm::vec3(0.0f),
        ConstraintSettings::Rigid());

    EXPECT_TRUE(constraint.IsWorldConstraint());
    EXPECT_FLOAT_EQ(constraint.minDistance, 3.0f);
    EXPECT_FLOAT_EQ(constraint.maxDistance, 3.0f);
}

//=============================================================================
// PointConstraint Component Tests
//=============================================================================

TEST(PointConstraintComponentTest, CreateBetweenBodies)
{
    Engine::Entity mockA(entt::entity(1));
    Engine::Entity mockB(entt::entity(2));

    auto constraint = PointConstraint::Create(
        mockA, mockB,
        glm::vec3(0.5f, 0.0f, 0.0f),
        glm::vec3(-0.5f, 0.0f, 0.0f),
        ConstraintSettings::Soft(0.8f, 0.2f));

    EXPECT_EQ(constraint.bodyA, mockA);
    EXPECT_EQ(constraint.bodyB, mockB);
    EXPECT_EQ(constraint.localPointA, glm::vec3(0.5f, 0.0f, 0.0f));
    EXPECT_EQ(constraint.localPointB, glm::vec3(-0.5f, 0.0f, 0.0f));
    EXPECT_FALSE(constraint.IsWorldConstraint());
    EXPECT_FALSE(constraint.broken);
}

TEST(PointConstraintComponentTest, CreateToWorld)
{
    Engine::Entity mockBody(entt::entity(1));

    auto constraint = PointConstraint::CreateToWorld(
        mockBody,
        glm::vec3(0.0f, 10.0f, 0.0f),
        ConstraintSettings::Rigid());

    EXPECT_TRUE(constraint.IsWorldConstraint());
    EXPECT_EQ(constraint.localPointB, glm::vec3(0.0f, 10.0f, 0.0f));  // World position
}

TEST(PointConstraintComponentTest, CreateToWorldWithOffset)
{
    Engine::Entity mockBody(entt::entity(1));

    auto constraint = PointConstraint::CreateToWorldWithOffset(
        mockBody,
        glm::vec3(0.0f, 10.0f, 0.0f),  // World anchor
        glm::vec3(0.0f, 1.0f, 0.0f),   // Local offset on body
        ConstraintSettings::Rigid());

    EXPECT_TRUE(constraint.IsWorldConstraint());
    EXPECT_EQ(constraint.localPointA, glm::vec3(0.0f, 1.0f, 0.0f));
    EXPECT_EQ(constraint.localPointB, glm::vec3(0.0f, 10.0f, 0.0f));
}

//=============================================================================
// ConstraintInternal Tests
//=============================================================================

TEST(ConstraintInternalTest, DefaultConstruction)
{
    ConstraintInternal internal;

    EXPECT_EQ(internal.constraint, nullptr);
    EXPECT_FALSE(internal.IsValid());
    EXPECT_FALSE(internal.broken);
    EXPECT_FALSE(internal.IsBreakable());
}

TEST(ConstraintInternalTest, ConstructWithBreakingThresholds)
{
    ConstraintInternal internal(nullptr, ConstraintType::Fixed, 100.0f, 50.0f);

    EXPECT_EQ(internal.type, ConstraintType::Fixed);
    EXPECT_FLOAT_EQ(internal.breakForce, 100.0f);
    EXPECT_FLOAT_EQ(internal.breakTorque, 50.0f);
    EXPECT_TRUE(internal.IsBreakable());
}

//=============================================================================
// Integration Tests - FixedConstraint with Physics System
//=============================================================================

TEST_F(BasicConstraintsTest, FixedConstraintCreatesInternalComponent)
{
    ASSERT_NE(internalA, nullptr);
    ASSERT_NE(internalB, nullptr);
    ASSERT_TRUE(internalA->IsValid());
    ASSERT_TRUE(internalB->IsValid());

    // Add fixed constraint
    auto constraint = FixedConstraint::Create(
        entityA, entityB,
        glm::vec3(0.0f, -0.5f, 0.0f),
        glm::vec3(0.0f, 0.5f, 0.0f),
        ConstraintSettings::Rigid());

    core->GetRegistry().emplace<FixedConstraint>(entityA, constraint);

    // Check that ConstraintInternal was created
    auto *constraintInternal = core->GetRegistry().try_get<ConstraintInternal>(entityA);
    ASSERT_NE(constraintInternal, nullptr);
    EXPECT_TRUE(constraintInternal->IsValid());
    EXPECT_EQ(constraintInternal->type, ConstraintType::Fixed);
}

TEST_F(BasicConstraintsTest, FixedConstraintRemovedOnDestroy)
{
    ASSERT_NE(internalA, nullptr);
    ASSERT_NE(internalB, nullptr);

    // Add then remove constraint
    auto constraint = FixedConstraint::Create(
        entityA, entityB,
        glm::vec3(0.0f), glm::vec3(0.0f),
        ConstraintSettings::Rigid());

    core->GetRegistry().emplace<FixedConstraint>(entityA, constraint);

    // Verify it exists
    ASSERT_NE(core->GetRegistry().try_get<ConstraintInternal>(entityA), nullptr);

    // Remove constraint
    core->GetRegistry().remove<FixedConstraint>(entityA);

    // Verify internal component is also removed
    EXPECT_EQ(core->GetRegistry().try_get<ConstraintInternal>(entityA), nullptr);
}

//=============================================================================
// Integration Tests - DistanceConstraint with Physics System
//=============================================================================

TEST_F(BasicConstraintsTest, DistanceConstraintCreatesInternalComponent)
{
    ASSERT_NE(internalA, nullptr);
    ASSERT_NE(internalB, nullptr);

    auto constraint = DistanceConstraint::Create(
        entityA, entityB,
        2.0f,  // Distance of 2 meters
        glm::vec3(0.0f), glm::vec3(0.0f),
        ConstraintSettings::Rigid());

    core->GetRegistry().emplace<DistanceConstraint>(entityA, constraint);

    auto *constraintInternal = core->GetRegistry().try_get<ConstraintInternal>(entityA);
    ASSERT_NE(constraintInternal, nullptr);
    EXPECT_TRUE(constraintInternal->IsValid());
    EXPECT_EQ(constraintInternal->type, ConstraintType::Distance);
}

TEST_F(BasicConstraintsTest, DistanceConstraintWithSoftLimits)
{
    ASSERT_NE(internalA, nullptr);
    ASSERT_NE(internalB, nullptr);

    auto constraint = DistanceConstraint::CreateWithRange(
        entityA, entityB,
        1.0f, 3.0f,  // Range 1-3 meters
        glm::vec3(0.0f), glm::vec3(0.0f),
        ConstraintSettings::Soft(0.5f, 0.1f));

    core->GetRegistry().emplace<DistanceConstraint>(entityA, constraint);

    auto *constraintInternal = core->GetRegistry().try_get<ConstraintInternal>(entityA);
    ASSERT_NE(constraintInternal, nullptr);
    EXPECT_TRUE(constraintInternal->IsValid());
}

//=============================================================================
// Integration Tests - PointConstraint with Physics System
//=============================================================================

TEST_F(BasicConstraintsTest, PointConstraintCreatesInternalComponent)
{
    ASSERT_NE(internalA, nullptr);
    ASSERT_NE(internalB, nullptr);

    auto constraint = PointConstraint::Create(
        entityA, entityB,
        glm::vec3(0.0f), glm::vec3(0.0f),
        ConstraintSettings::Rigid());

    core->GetRegistry().emplace<PointConstraint>(entityA, constraint);

    auto *constraintInternal = core->GetRegistry().try_get<ConstraintInternal>(entityA);
    ASSERT_NE(constraintInternal, nullptr);
    EXPECT_TRUE(constraintInternal->IsValid());
    EXPECT_EQ(constraintInternal->type, ConstraintType::Point);
}

TEST_F(BasicConstraintsTest, PointConstraintToWorld)
{
    ASSERT_NE(internalB, nullptr);

    auto constraint = PointConstraint::CreateToWorld(
        entityB,
        glm::vec3(0.0f, 10.0f, 0.0f),  // World anchor
        ConstraintSettings::Rigid());

    core->GetRegistry().emplace<PointConstraint>(entityB, constraint);

    auto *constraintInternal = core->GetRegistry().try_get<ConstraintInternal>(entityB);
    ASSERT_NE(constraintInternal, nullptr);
    EXPECT_TRUE(constraintInternal->IsValid());
}

//=============================================================================
// Error Handling Tests
//=============================================================================

TEST_F(BasicConstraintsTest, ConstraintWithInvalidBodyAFails)
{
    // Create a valid entity to hold the constraint
    Engine::Entity constraintEntity(core->CreateEntity());
    
    // Reference an invalid entity as bodyA (doesn't exist, no RigidBody)
    Engine::Entity invalidBodyA(entt::entity(99999));

    auto constraint = FixedConstraint::Create(
        invalidBodyA, entityB,
        glm::vec3(0.0f), glm::vec3(0.0f),
        ConstraintSettings::Rigid());

    // Add constraint to a valid entity, but with an invalid bodyA reference
    core->GetRegistry().emplace<FixedConstraint>(constraintEntity, constraint);

    // ConstraintInternal should NOT be created (error case - bodyA is invalid)
    auto *constraintInternal = core->GetRegistry().try_get<ConstraintInternal>(constraintEntity);
    EXPECT_EQ(constraintInternal, nullptr);
}
