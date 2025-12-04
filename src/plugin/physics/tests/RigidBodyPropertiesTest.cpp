/*
** EPITECH PROJECT, 2025
** EngineSquared
** File description:
** RigidBody Properties Test
*/

#include <gtest/gtest.h>

#include "Engine.hpp"
#include "Object.hpp"
#include "Physics.hpp"

using namespace Physics::Component;
using namespace Physics::Resource;

TEST(RigidBodyPropertiesTest, ApplyPropertiesToJoltBody)
{
    Engine::Core core;

    // Register physics plugin and run startup systems
    core.AddPlugins<Physics::Plugin>();
    core.RunSystems();

    auto &registry = core.GetRegistry();

    // Create entity and add components
    Engine::Entity ent(core.CreateEntity());

    Object::Component::Transform transform;
    transform.SetPosition(glm::vec3(0.0f, 5.0f, 0.0f));
    registry.emplace<Object::Component::Transform>(ent, transform);

    // Create a RigidBody with explicit properties and emplace it (so OnRigidBodyConstruct uses these values)
    Physics::Component::RigidBody rbData;
    rbData.motionType = MotionType::Dynamic;
    rbData.mass = 2.0f;
    rbData.friction = 0.75f;
    rbData.restitution = 0.25f;
    rbData.linearDamping = 0.12f;
    rbData.angularDamping = 0.18f;
    rbData.gravityFactor = 0.5f;
    rbData.motionQuality = MotionQuality::Discrete;
    rbData.useMotionQualityLinearCast = true; // override -> LinearCast
    rbData.enhancedInternalEdgeRemoval = true;
    rbData.lockPositionZ = true; // lock Z translation
    registry.emplace<RigidBody>(ent, rbData);

    // PhysicsManager is available via resource
    auto &physicsManager = core.GetResource<PhysicsManager>();

    // Check that RigidBodyInternal exists
    auto *internal = registry.try_get<RigidBodyInternal>(ent);
    ASSERT_NE(internal, nullptr);
    ASSERT_TRUE(internal->IsValid());

    // Find Jolt body
    // Use BodyLockInterface to safely access the body pointer
    auto &bodyLockInterface = physicsManager.GetPhysicsSystem().GetBodyLockInterface();
    JPH::Body *body = bodyLockInterface.TryGetBody(internal->bodyID);
    ASSERT_NE(body, nullptr);

    // Validate properties - read the stored component from the registry
    auto &storedRb = registry.get<RigidBody>(ent);
    EXPECT_NEAR(body->GetFriction(), storedRb.friction, 1e-6);
    EXPECT_NEAR(body->GetRestitution(), storedRb.restitution, 1e-6);

    const JPH::MotionProperties *mp = body->GetMotionProperties();
    ASSERT_NE(mp, nullptr);
    EXPECT_NEAR(mp->GetLinearDamping(), storedRb.linearDamping, 1e-6);
    EXPECT_NEAR(mp->GetAngularDamping(), storedRb.angularDamping, 1e-6);
    EXPECT_NEAR(mp->GetGravityFactor(), storedRb.gravityFactor, 1e-6);

    // Motion quality: because useMotionQualityLinearCast = true
    EXPECT_EQ(mp->GetMotionQuality(), JPH::EMotionQuality::LinearCast);

    // Enhanced internal edge removal
    EXPECT_TRUE(body->GetEnhancedInternalEdgeRemoval());

    // Check allowed DOFs: TranslationZ should be locked
    auto allowed = mp->GetAllowedDOFs();
    EXPECT_FALSE(static_cast<bool>(allowed & JPH::EAllowedDOFs::TranslationZ));
}
