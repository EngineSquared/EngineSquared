/*
** EPITECH PROJECT, 2025
** EngineSquared
** File description:
** RigidBody Runtime Setters Test
*/

#include <gtest/gtest.h>

#include "Engine.hpp"
#include "Object.hpp"
#include "Physics.hpp"
#include "helper/RigidBodyController.hpp"

using namespace Physics::Component;
using namespace Physics::Resource;

class RigidBodyRuntimeSettersTest : public ::testing::Test {
  protected:
    void SetUp() override
    {
        core = std::make_unique<Engine::Core>();
        core->AddPlugins<Physics::Plugin>();
        core->RunSystems();

        entity = Engine::Entity(core->CreateEntity());

        Object::Component::Transform transform;
        transform.SetPosition(glm::vec3(0.0f, 2.0f, 0.0f));
        core->GetRegistry().emplace<Object::Component::Transform>(entity, transform);

        Physics::Component::RigidBody rbData;
        rbData.motionType = MotionType::Dynamic;
        rbData.mass = 1.0f;
        rbData.friction = 0.5f;
        rbData.restitution = 0.1f;
        core->GetRegistry().emplace<RigidBody>(entity, rbData);

        internal = core->GetRegistry().try_get<RigidBodyInternal>(entity);
    }

    void TearDown() override { core.reset(); }

    std::unique_ptr<Engine::Core> core;
    Engine::Entity entity{0};
    RigidBodyInternal *internal = nullptr;
};

TEST_F(RigidBodyRuntimeSettersTest, SetFriction)
{
    ASSERT_NE(internal, nullptr);
    ASSERT_TRUE(internal->IsValid());

    auto &physicsManager = core->GetResource<PhysicsManager>();

    Physics::Helper::SetFriction(*core, entity, 0.2f);

    EXPECT_NEAR(physicsManager.GetBodyInterface().GetFriction(internal->bodyID), 0.2f, 1e-6);
    EXPECT_NEAR(core->GetRegistry().get<RigidBody>(entity).friction, 0.2f, 1e-6);
}

TEST_F(RigidBodyRuntimeSettersTest, SetRestitution)
{
    ASSERT_NE(internal, nullptr);
    ASSERT_TRUE(internal->IsValid());

    auto &physicsManager = core->GetResource<PhysicsManager>();

    Physics::Helper::SetRestitution(*core, entity, 0.6f);

    EXPECT_NEAR(physicsManager.GetBodyInterface().GetRestitution(internal->bodyID), 0.6f, 1e-6);
    EXPECT_NEAR(core->GetRegistry().get<RigidBody>(entity).restitution, 0.6f, 1e-6);
}

TEST_F(RigidBodyRuntimeSettersTest, SetMotionQualityLinearCast)
{
    ASSERT_NE(internal, nullptr);
    ASSERT_TRUE(internal->IsValid());

    auto &physicsManager = core->GetResource<PhysicsManager>();

    Physics::Helper::SetMotionQuality(*core, entity, JPH::EMotionQuality::Discrete, true);

    EXPECT_EQ(physicsManager.GetBodyInterface().GetMotionQuality(internal->bodyID), JPH::EMotionQuality::LinearCast);
}

TEST_F(RigidBodyRuntimeSettersTest, SetEnhancedInternalEdgeRemoval)
{
    ASSERT_NE(internal, nullptr);
    ASSERT_TRUE(internal->IsValid());

    auto &physicsManager = core->GetResource<PhysicsManager>();

    Physics::Helper::SetEnhancedInternalEdgeRemoval(*core, entity, true);

    auto &bodyLockInterface = physicsManager.GetPhysicsSystem().GetBodyLockInterface();
    JPH::Body *body = bodyLockInterface.TryGetBody(internal->bodyID);
    ASSERT_NE(body, nullptr);
    EXPECT_TRUE(body->GetEnhancedInternalEdgeRemoval());
}

TEST_F(RigidBodyRuntimeSettersTest, SetAxisLocksPositionZ)
{
    ASSERT_NE(internal, nullptr);
    ASSERT_TRUE(internal->IsValid());

    auto &physicsManager = core->GetResource<PhysicsManager>();

    Physics::Helper::SetAxisLocks(*core, entity, false, false, true, false, false, false);

    auto &bodyLockInterface = physicsManager.GetPhysicsSystem().GetBodyLockInterface();
    JPH::Body *body = bodyLockInterface.TryGetBody(internal->bodyID);
    ASSERT_NE(body, nullptr);
    auto mp = body->GetMotionProperties();
    ASSERT_NE(mp, nullptr);
    EXPECT_FALSE(static_cast<bool>(mp->GetAllowedDOFs() & JPH::EAllowedDOFs::TranslationZ));
}

TEST_F(RigidBodyRuntimeSettersTest, SetLinearDamping)
{
    ASSERT_NE(internal, nullptr);
    ASSERT_TRUE(internal->IsValid());

    auto &physicsManager = core->GetResource<PhysicsManager>();

    Physics::Helper::SetLinearDamping(*core, entity, 0.75f);

    auto &bodyLockInterface = physicsManager.GetPhysicsSystem().GetBodyLockInterface();
    JPH::Body *body = bodyLockInterface.TryGetBody(internal->bodyID);
    ASSERT_NE(body, nullptr);
    auto mp = body->GetMotionProperties();
    ASSERT_NE(mp, nullptr);
    EXPECT_NEAR(mp->GetLinearDamping(), 0.75f, 1e-6);
    EXPECT_NEAR(core->GetRegistry().get<RigidBody>(entity).linearDamping, 0.75f, 1e-6);
}

TEST_F(RigidBodyRuntimeSettersTest, SetAngularDamping)
{
    ASSERT_NE(internal, nullptr);
    ASSERT_TRUE(internal->IsValid());

    auto &physicsManager = core->GetResource<PhysicsManager>();

    Physics::Helper::SetAngularDamping(*core, entity, 0.25f);

    auto &bodyLockInterface = physicsManager.GetPhysicsSystem().GetBodyLockInterface();
    JPH::Body *body = bodyLockInterface.TryGetBody(internal->bodyID);
    ASSERT_NE(body, nullptr);
    auto mp = body->GetMotionProperties();
    ASSERT_NE(mp, nullptr);
    EXPECT_NEAR(mp->GetAngularDamping(), 0.25f, 1e-6);
    EXPECT_NEAR(core->GetRegistry().get<RigidBody>(entity).angularDamping, 0.25f, 1e-6);
}

TEST_F(RigidBodyRuntimeSettersTest, SetMass)
{
    ASSERT_NE(internal, nullptr);
    ASSERT_TRUE(internal->IsValid());

    auto &physicsManager = core->GetResource<PhysicsManager>();

    Physics::Helper::SetMass(*core, entity, 2.0f);

    auto &bodyLockInterface = physicsManager.GetPhysicsSystem().GetBodyLockInterface();
    JPH::Body *body = bodyLockInterface.TryGetBody(internal->bodyID);
    ASSERT_NE(body, nullptr);
    auto mp = body->GetMotionProperties();
    ASSERT_NE(mp, nullptr);
    EXPECT_NEAR(mp->GetInverseMass(), 1.0f / 2.0f, 1e-6);
    EXPECT_NEAR(core->GetRegistry().get<RigidBody>(entity).mass, 2.0f, 1e-6);
}

TEST_F(RigidBodyRuntimeSettersTest, SetMotionType)
{
    ASSERT_NE(internal, nullptr);
    ASSERT_TRUE(internal->IsValid());

    auto &physicsManager = core->GetResource<PhysicsManager>();

    Physics::Helper::SetMotionType(*core, entity, JPH::EMotionType::Kinematic, JPH::EActivation::DontActivate);

    EXPECT_EQ(physicsManager.GetBodyInterface().GetMotionType(internal->bodyID), JPH::EMotionType::Kinematic);
}

TEST_F(RigidBodyRuntimeSettersTest, SetAllowSleepingDisabled)
{
    ASSERT_NE(internal, nullptr);
    ASSERT_TRUE(internal->IsValid());

    auto &physicsManager = core->GetResource<PhysicsManager>();

    Physics::Helper::SetAllowSleeping(*core, entity, false);

    auto &bodyLockInterface = physicsManager.GetPhysicsSystem().GetBodyLockInterface();
    JPH::Body *body = bodyLockInterface.TryGetBody(internal->bodyID);
    ASSERT_NE(body, nullptr);
    EXPECT_FALSE(body->GetAllowSleeping());
    EXPECT_FALSE(core->GetRegistry().get<RigidBody>(entity).allowSleeping);
}

TEST_F(RigidBodyRuntimeSettersTest, SetAllowSleepingEnabled)
{
    ASSERT_NE(internal, nullptr);
    ASSERT_TRUE(internal->IsValid());

    auto &physicsManager = core->GetResource<PhysicsManager>();

    // First disable, then re-enable
    Physics::Helper::SetAllowSleeping(*core, entity, false);
    Physics::Helper::SetAllowSleeping(*core, entity, true);

    auto &bodyLockInterface = physicsManager.GetPhysicsSystem().GetBodyLockInterface();
    JPH::Body *body = bodyLockInterface.TryGetBody(internal->bodyID);
    ASSERT_NE(body, nullptr);
    EXPECT_TRUE(body->GetAllowSleeping());
    EXPECT_TRUE(core->GetRegistry().get<RigidBody>(entity).allowSleeping);
}
