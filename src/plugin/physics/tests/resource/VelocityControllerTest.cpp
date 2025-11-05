/*
** EPITECH PROJECT, 2025
** EngineSquared
** File description:
** VelocityControllerTest - Test velocity control API (Issue #002)
*/

#include <gtest/gtest.h>
#define GLM_ENABLE_EXPERIMENTAL
#include "Engine.hpp"
#include "Physics.hpp"
#include "Object.hpp"

using namespace Physics::Resource;
using namespace Physics::Component;

class VelocityControllerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create engine core with Physics plugin
        core.AddPlugins<Physics::Plugin>();

        // Create a test entity with RigidBody (Dynamic)
        testEntity = Engine::Entity(core.CreateEntity());

        // Add Transform (required by physics system)
        auto transform = Object::Component::Transform();
        transform.SetPosition(0.0f, 5.0f, 0.0f);
        core.GetRegistry().emplace<Object::Component::Transform>(testEntity, transform);

        // Add RigidBody (Dynamic)
        auto& rb = core.GetRegistry().emplace<RigidBody>(testEntity);
        rb.motionType = MotionType::Dynamic;
        rb.mass = 2.0f;
        rb.friction = 0.5f;
        rb.restitution = 0.3f;
        rb.linearDamping = 0.0f;  // No damping for predictable tests
        rb.angularDamping = 0.0f;

        // Create static entity for negative tests
        staticEntity = Engine::Entity(core.CreateEntity());

        auto staticTransform = Object::Component::Transform();
        staticTransform.SetPosition(0.0f, 0.0f, 0.0f);
        core.GetRegistry().emplace<Object::Component::Transform>(staticEntity, staticTransform);

        auto& staticRb = core.GetRegistry().emplace<RigidBody>(staticEntity);
        staticRb.motionType = MotionType::Static;

        // Note: In real usage, RigidBodyInternal would be created by the physics system
        // For these tests we just verify the API doesn't crash
    }

    Engine::Core core;
    Engine::Entity testEntity;
    Engine::Entity staticEntity;
};

//============================================================================
// API Existence Tests - Verify functions compile and don't crash
//============================================================================

TEST_F(VelocityControllerTest, SetLinearVelocity_CompileAndExecute) {
    glm::vec3 velocity(5.0f, 0.0f, 0.0f);

    // Should compile and not crash (may log warning about missing RigidBodyInternal)
    EXPECT_NO_THROW(SetLinearVelocity(core, testEntity, velocity));
}

TEST_F(VelocityControllerTest, GetLinearVelocity_CompileAndExecute) {
    // Should compile and not crash
    glm::vec3 velocity;
    EXPECT_NO_THROW(velocity = GetLinearVelocity(core, testEntity));

    // Without RigidBodyInternal, should return zero
    EXPECT_EQ(velocity, glm::vec3(0.0f));
}

TEST_F(VelocityControllerTest, AddLinearVelocity_CompileAndExecute) {
    glm::vec3 deltaVelocity(2.0f, 0.0f, 0.0f);

    EXPECT_NO_THROW(AddLinearVelocity(core, testEntity, deltaVelocity));
}

TEST_F(VelocityControllerTest, SetAngularVelocity_CompileAndExecute) {
    glm::vec3 angularVelocity(0.0f, 2.0f, 0.0f);

    EXPECT_NO_THROW(SetAngularVelocity(core, testEntity, angularVelocity));
}

TEST_F(VelocityControllerTest, GetAngularVelocity_CompileAndExecute) {
    // Should compile and not crash
    glm::vec3 angularVelocity;
    EXPECT_NO_THROW(angularVelocity = GetAngularVelocity(core, testEntity));

    // Without RigidBodyInternal, should return zero
    EXPECT_EQ(angularVelocity, glm::vec3(0.0f));
}

TEST_F(VelocityControllerTest, AddAngularVelocity_CompileAndExecute) {
    glm::vec3 deltaAngularVelocity(0.0f, 1.0f, 0.0f);

    EXPECT_NO_THROW(AddAngularVelocity(core, testEntity, deltaAngularVelocity));
}

//============================================================================
// ERROR HANDLING Tests
//============================================================================

TEST_F(VelocityControllerTest, InvalidEntity_DoesNotCrash) {
    Engine::Entity invalidEntity = Engine::Entity(entt::entity(999999));

    EXPECT_NO_THROW(SetLinearVelocity(core, invalidEntity, glm::vec3(1.0f, 0.0f, 0.0f)));

    glm::vec3 velocity = GetLinearVelocity(core, invalidEntity);
    EXPECT_EQ(velocity, glm::vec3(0.0f));

    EXPECT_NO_THROW(AddLinearVelocity(core, invalidEntity, glm::vec3(1.0f, 0.0f, 0.0f)));

    EXPECT_NO_THROW(SetAngularVelocity(core, invalidEntity, glm::vec3(0.0f, 1.0f, 0.0f)));

    glm::vec3 angVel = GetAngularVelocity(core, invalidEntity);
    EXPECT_EQ(angVel, glm::vec3(0.0f));

    EXPECT_NO_THROW(AddAngularVelocity(core, invalidEntity, glm::vec3(0.0f, 1.0f, 0.0f)));
}

TEST_F(VelocityControllerTest, NaNVector_DoesNotCrash) {
    glm::vec3 nanVelocity(std::numeric_limits<float>::quiet_NaN(), 0.0f, 0.0f);

    // Should handle gracefully (log error and return)
    EXPECT_NO_THROW(SetLinearVelocity(core, testEntity, nanVelocity));
    EXPECT_NO_THROW(SetAngularVelocity(core, testEntity, nanVelocity));
}

TEST_F(VelocityControllerTest, InfVector_DoesNotCrash) {
    glm::vec3 infVelocity(std::numeric_limits<float>::infinity(), 0.0f, 0.0f);

    EXPECT_NO_THROW(SetLinearVelocity(core, testEntity, infVelocity));
    EXPECT_NO_THROW(SetAngularVelocity(core, testEntity, infVelocity));
}

TEST_F(VelocityControllerTest, StaticBody_RejectsVelocityChange) {
    glm::vec3 velocity(5.0f, 0.0f, 0.0f);

    // Should log warning and not crash
    EXPECT_NO_THROW(SetLinearVelocity(core, staticEntity, velocity));
    EXPECT_NO_THROW(SetAngularVelocity(core, staticEntity, glm::vec3(0.0f, 1.0f, 0.0f)));
}
