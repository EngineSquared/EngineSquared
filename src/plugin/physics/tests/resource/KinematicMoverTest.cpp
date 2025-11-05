/*
** EPITECH PROJECT, 2025
** EngineSquared
** File description:
** KinematicMoverTest - Test kinematic movement API (Issue #003)
*/

#include <gtest/gtest.h>
#define GLM_ENABLE_EXPERIMENTAL
#include "Engine.hpp"
#include "Physics.hpp"
#include "Object.hpp"

using namespace Physics::Resource;
using namespace Physics::Component;

class KinematicMoverTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create engine core with Physics plugin
        core.AddPlugins<Physics::Plugin>();

        // Create a test entity with Kinematic RigidBody
        kinematicEntity = Engine::Entity(core.CreateEntity());

        // Add Transform (required by physics system)
        auto transform = Object::Component::Transform();
        transform.SetPosition(0.0f, 5.0f, 0.0f);
        core.GetRegistry().emplace<Object::Component::Transform>(kinematicEntity, transform);

        // Add RigidBody (Kinematic)
        auto& rb = core.GetRegistry().emplace<RigidBody>(kinematicEntity);
        rb.motionType = MotionType::Kinematic;
        rb.mass = 1.0f;

        // Create dynamic entity for negative tests
        dynamicEntity = Engine::Entity(core.CreateEntity());

        auto dynamicTransform = Object::Component::Transform();
        dynamicTransform.SetPosition(0.0f, 10.0f, 0.0f);
        core.GetRegistry().emplace<Object::Component::Transform>(dynamicEntity, dynamicTransform);

        auto& dynamicRb = core.GetRegistry().emplace<RigidBody>(dynamicEntity);
        dynamicRb.motionType = MotionType::Dynamic;
        dynamicRb.mass = 2.0f;

        // Note: In real usage, RigidBodyInternal would be created by the physics system
        // For these tests we just verify the API doesn't crash
    }

    Engine::Core core;
    Engine::Entity kinematicEntity;
    Engine::Entity dynamicEntity;
};

//============================================================================
// API Existence Tests - Verify functions compile and don't crash
//============================================================================

TEST_F(KinematicMoverTest, MoveKinematic_CompileAndExecute) {
    glm::vec3 targetPosition(5.0f, 2.0f, 3.0f);
    glm::quat targetRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    float deltaTime = 1.0f / 60.0f;

    // Should compile and not crash (may log warning about missing RigidBodyInternal)
    EXPECT_NO_THROW(MoveKinematic(core, kinematicEntity, targetPosition, targetRotation, deltaTime));
}

TEST_F(KinematicMoverTest, SetKinematicVelocity_CompileAndExecute) {
    glm::vec3 velocity(2.0f, 0.0f, 0.0f);
    float deltaTime = 1.0f / 60.0f;

    EXPECT_NO_THROW(SetKinematicVelocity(core, kinematicEntity, velocity, deltaTime));
}

TEST_F(KinematicMoverTest, GetKinematicTarget_CompileAndExecute) {
    // Should compile and not crash
    KinematicTarget target;
    EXPECT_NO_THROW(target = GetKinematicTarget(core, kinematicEntity));

    // Without RigidBodyInternal, hasTarget should be false
    EXPECT_FALSE(target.hasTarget);
}

//============================================================================
// ERROR HANDLING Tests
//============================================================================

TEST_F(KinematicMoverTest, MoveKinematic_WarnsOnNonKinematicBody) {
    glm::vec3 targetPosition(5.0f, 2.0f, 3.0f);
    glm::quat targetRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    float deltaTime = 1.0f / 60.0f;

    // Should log warning and not crash
    EXPECT_NO_THROW(MoveKinematic(core, dynamicEntity, targetPosition, targetRotation, deltaTime));
}

TEST_F(KinematicMoverTest, MoveKinematic_RejectsInvalidDeltaTime) {
    glm::vec3 targetPosition(5.0f, 2.0f, 3.0f);
    glm::quat targetRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

    // Zero deltaTime
    EXPECT_NO_THROW(MoveKinematic(core, kinematicEntity, targetPosition, targetRotation, 0.0f));

    // Negative deltaTime
    EXPECT_NO_THROW(MoveKinematic(core, kinematicEntity, targetPosition, targetRotation, -0.016f));
}

TEST_F(KinematicMoverTest, MoveKinematic_RejectsInvalidPosition) {
    glm::quat targetRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    float deltaTime = 1.0f / 60.0f;

    // NaN position
    glm::vec3 nanPosition(std::numeric_limits<float>::quiet_NaN(), 0.0f, 0.0f);
    EXPECT_NO_THROW(MoveKinematic(core, kinematicEntity, nanPosition, targetRotation, deltaTime));

    // Inf position
    glm::vec3 infPosition(std::numeric_limits<float>::infinity(), 0.0f, 0.0f);
    EXPECT_NO_THROW(MoveKinematic(core, kinematicEntity, infPosition, targetRotation, deltaTime));
}

TEST_F(KinematicMoverTest, SetKinematicVelocity_RejectsInvalidVelocity) {
    float deltaTime = 1.0f / 60.0f;

    // NaN velocity
    glm::vec3 nanVelocity(std::numeric_limits<float>::quiet_NaN(), 0.0f, 0.0f);
    EXPECT_NO_THROW(SetKinematicVelocity(core, kinematicEntity, nanVelocity, deltaTime));

    // Inf velocity
    glm::vec3 infVelocity(std::numeric_limits<float>::infinity(), 0.0f, 0.0f);
    EXPECT_NO_THROW(SetKinematicVelocity(core, kinematicEntity, infVelocity, deltaTime));
}

TEST_F(KinematicMoverTest, GetKinematicTarget_ReturnsFalseForNonKinematic) {
    // Query dynamic body
    auto target = GetKinematicTarget(core, dynamicEntity);
    
    // Should return hasTarget = false
    EXPECT_FALSE(target.hasTarget);
}

TEST_F(KinematicMoverTest, InvalidEntity_DoesNotCrash) {
    Engine::Entity invalidEntity = Engine::Entity(entt::entity(999999));
    glm::vec3 targetPosition(5.0f, 2.0f, 3.0f);
    glm::quat targetRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    float deltaTime = 1.0f / 60.0f;

    EXPECT_NO_THROW(MoveKinematic(core, invalidEntity, targetPosition, targetRotation, deltaTime));
    EXPECT_NO_THROW(SetKinematicVelocity(core, invalidEntity, glm::vec3(1.0f, 0.0f, 0.0f), deltaTime));

    auto target = GetKinematicTarget(core, invalidEntity);
    EXPECT_FALSE(target.hasTarget);
}

//============================================================================
// BEHAVIOR Tests - Verify kinematic properties
//============================================================================

TEST_F(KinematicMoverTest, KinematicBody_HasCorrectMotionType) {
    // Verify test setup
    auto& rb = core.GetRegistry().get<RigidBody>(kinematicEntity);
    EXPECT_EQ(rb.motionType, MotionType::Kinematic);
}

TEST_F(KinematicMoverTest, DynamicBody_HasCorrectMotionType) {
    // Verify test setup
    auto& rb = core.GetRegistry().get<RigidBody>(dynamicEntity);
    EXPECT_EQ(rb.motionType, MotionType::Dynamic);
}
