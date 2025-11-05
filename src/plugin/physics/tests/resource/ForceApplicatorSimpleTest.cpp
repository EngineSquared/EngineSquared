/*
** EPITECH PROJECT, 2025
** EngineSquared
** File description:
** ForceApplicatorSimpleTest - Basic functionality tests
*/

#include <gtest/gtest.h>
#define GLM_ENABLE_EXPERIMENTAL
#include "Engine.hpp"
#include "Physics.hpp"

using namespace Physics::Resource;
using namespace Physics::Component;

class ForceApplicatorSimpleTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create engine core with Physics plugin
        core.AddPlugins<Physics::Plugin>();

        // Create a test entity with RigidBody
        testEntity = Engine::Entity(core.CreateEntity());

        auto& rb = core.GetRegistry().emplace<RigidBody>(testEntity);
        rb.motionType = MotionType::Dynamic;
        rb.mass = 1.0f;
        rb.friction = 0.5f;
        rb.restitution = 0.3f;
        rb.linearDamping = 0.05f;
        rb.angularDamping = 0.05f;

        // Note: In real usage, RigidBodyInternal would be created by the physics system
        // For this test we just verify the API doesn't crash
    }

    Engine::Core core;
    Engine::Entity testEntity;
};

//============================================================================
// API Existence Tests - Verify functions compile and don't crash
//============================================================================

TEST_F(ForceApplicatorSimpleTest, AddForce_CompileAndExecute) {
    glm::vec3 force(0.0f, 10.0f, 0.0f);

    // Should compile and not crash (may log warning about missing RigidBodyInternal)
    EXPECT_NO_THROW(AddForce(core, testEntity, force));
}

TEST_F(ForceApplicatorSimpleTest, AddForceAtPoint_CompileAndExecute) {
    glm::vec3 force(10.0f, 0.0f, 0.0f);
    glm::vec3 worldPoint(0.0f, 1.0f, 0.0f);

    EXPECT_NO_THROW(AddForceAtPoint(core, testEntity, force, worldPoint));
}

TEST_F(ForceApplicatorSimpleTest, AddTorque_CompileAndExecute) {
    glm::vec3 torque(0.0f, 5.0f, 0.0f);

    EXPECT_NO_THROW(AddTorque(core, testEntity, torque));
}

TEST_F(ForceApplicatorSimpleTest, AddImpulse_CompileAndExecute) {
    glm::vec3 impulse(5.0f, 0.0f, 0.0f);

    EXPECT_NO_THROW(AddImpulse(core, testEntity, impulse));
}

TEST_F(ForceApplicatorSimpleTest, AddImpulseAtPoint_CompileAndExecute) {
    glm::vec3 impulse(10.0f, 0.0f, 0.0f);
    glm::vec3 worldPoint(0.0f, 1.0f, 0.0f);

    EXPECT_NO_THROW(AddImpulseAtPoint(core, testEntity, impulse, worldPoint));
}

TEST_F(ForceApplicatorSimpleTest, AddAngularImpulse_CompileAndExecute) {
    glm::vec3 angularImpulse(0.0f, 10.0f, 0.0f);

    EXPECT_NO_THROW(AddAngularImpulse(core, testEntity, angularImpulse));
}

//============================================================================
// Error Handling Tests
//============================================================================

TEST_F(ForceApplicatorSimpleTest, InvalidEntity_DoesNotCrash) {
    Engine::Entity invalidEntity = Engine::Entity(entt::entity(999999));

    EXPECT_NO_THROW(AddForce(core, invalidEntity, glm::vec3(1.0f, 0.0f, 0.0f)));
    EXPECT_NO_THROW(AddForceAtPoint(core, invalidEntity, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f)));
    EXPECT_NO_THROW(AddTorque(core, invalidEntity, glm::vec3(0.0f, 1.0f, 0.0f)));
    EXPECT_NO_THROW(AddImpulse(core, invalidEntity, glm::vec3(1.0f, 0.0f, 0.0f)));
    EXPECT_NO_THROW(AddImpulseAtPoint(core, invalidEntity, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f)));
    EXPECT_NO_THROW(AddAngularImpulse(core, invalidEntity, glm::vec3(0.0f, 1.0f, 0.0f)));
}

TEST_F(ForceApplicatorSimpleTest, NaNVector_DoesNotCrash) {
    glm::vec3 nanForce(std::numeric_limits<float>::quiet_NaN(), 0.0f, 0.0f);

    EXPECT_NO_THROW(AddForce(core, testEntity, nanForce));
    EXPECT_NO_THROW(AddImpulse(core, testEntity, nanForce));
    EXPECT_NO_THROW(AddTorque(core, testEntity, nanForce));
}

TEST_F(ForceApplicatorSimpleTest, InfinityVector_DoesNotCrash) {
    glm::vec3 infForce(std::numeric_limits<float>::infinity(), 0.0f, 0.0f);

    EXPECT_NO_THROW(AddForce(core, testEntity, infForce));
    EXPECT_NO_THROW(AddImpulse(core, testEntity, infForce));
}

TEST_F(ForceApplicatorSimpleTest, StaticBody_DoesNotCrash) {
    // Create static body
    Engine::Entity staticEntity = Engine::Entity(core.CreateEntity());
    auto& rbStatic = core.GetRegistry().emplace<RigidBody>(staticEntity);
    rbStatic.motionType = MotionType::Static;

    EXPECT_NO_THROW(AddForce(core, staticEntity, glm::vec3(1.0f, 0.0f, 0.0f)));
    EXPECT_NO_THROW(AddImpulse(core, staticEntity, glm::vec3(1.0f, 0.0f, 0.0f)));
}

//============================================================================
// API Signature Tests - Verify correct types
//============================================================================

TEST_F(ForceApplicatorSimpleTest, CorrectSignatures) {
    // Verify functions accept the correct types
    Engine::Core& coreRef = core;
    Engine::Entity entityCopy = testEntity;
    const glm::vec3 constForce(1.0f, 2.0f, 3.0f);
    glm::vec3 mutableForce(1.0f, 2.0f, 3.0f);

    // Should all compile without errors
    AddForce(coreRef, entityCopy, constForce);
    AddForce(core, testEntity, mutableForce);
    AddForceAtPoint(core, testEntity, constForce, glm::vec3(0.0f));
    AddTorque(core, testEntity, constForce);
    AddImpulse(core, testEntity, constForce);
    AddImpulseAtPoint(core, testEntity, constForce, glm::vec3(0.0f));
    AddAngularImpulse(core, testEntity, constForce);
}
