/*
** EPITECH PROJECT, 2025
** EngineSquared
** File description:
** SoftBody Tests - Issue #021
*/

#include <gtest/gtest.h>

#include "Engine.hpp"
#include "Object.hpp"
#include "Physics.hpp"

using namespace Physics::Component;
using namespace Physics::Resource;

//=============================================================================
// Unit Tests - SoftBodySettings
//=============================================================================

TEST(SoftBodySettingsTest, DefaultConstruction)
{
    SoftBodySettings settings;

    EXPECT_EQ(settings.solverIterations, 5u);
    EXPECT_FLOAT_EQ(settings.linearDamping, 0.1f);
    EXPECT_FLOAT_EQ(settings.gravityFactor, 1.0f);
    EXPECT_FLOAT_EQ(settings.friction, 0.2f);
    EXPECT_FLOAT_EQ(settings.pressure, 0.0f);
    EXPECT_TRUE(settings.allowSleeping);
}

TEST(SoftBodySettingsTest, ClothFactory)
{
    auto settings = SoftBodySettings::Cloth(0.5f);

    EXPECT_EQ(settings.solverIterations, 8u);
    EXPECT_GT(settings.linearDamping, 0.1f); // Higher than default
    EXPECT_TRUE(settings.doubleSidedFaces);
}

TEST(SoftBodySettingsTest, RopeFactory)
{
    auto settings = SoftBodySettings::Rope(0.9f);

    EXPECT_EQ(settings.solverIterations, 10u);
    EXPECT_GT(settings.linearDamping, 0.2f); // Higher damping for rope
}

TEST(SoftBodySettingsTest, BalloonFactory)
{
    auto settings = SoftBodySettings::Balloon(2000.0f);

    EXPECT_FLOAT_EQ(settings.pressure, 2000.0f);
    EXPECT_GT(settings.restitution, 0.0f); // Bouncy
}

TEST(SoftBodySettingsTest, JellyFactory)
{
    auto settings = SoftBodySettings::Jelly();

    EXPECT_GT(settings.pressure, 0.0f);
    EXPECT_GT(settings.restitution, 0.5f); // Very bouncy
}

//=============================================================================
// Unit Tests - SoftBody Component
//=============================================================================

TEST(SoftBodyTest, CreateCloth)
{
    auto cloth = SoftBody::CreateCloth(5, 5, 0.1f);

    EXPECT_EQ(cloth.type, SoftBodyType::Cloth);
    EXPECT_EQ(cloth.GetVertexCount(), 25u); // 5 * 5
    EXPECT_EQ(cloth.GetFaceCount(), 32u);   // (5-1) * (5-1) * 2 = 32
    EXPECT_GT(cloth.GetEdgeCount(), 0u);
    EXPECT_TRUE(cloth.IsValid());
}

TEST(SoftBodyTest, CreateRope)
{
    auto rope = SoftBody::CreateRope(10, 0.1f);

    EXPECT_EQ(rope.type, SoftBodyType::Rope);
    EXPECT_EQ(rope.GetVertexCount(), 11u); // 10 segments + 1
    EXPECT_EQ(rope.GetFaceCount(), 0u);    // No faces for rope
    EXPECT_EQ(rope.GetEdgeCount(), 10u);   // 10 segments
    EXPECT_TRUE(rope.IsValid());
}

TEST(SoftBodyTest, CreateCube)
{
    auto cube = SoftBody::CreateCube(3, 0.1f);

    EXPECT_EQ(cube.type, SoftBodyType::Cube);
    EXPECT_EQ(cube.GetVertexCount(), 27u); // 3 * 3 * 3
    EXPECT_GT(cube.GetEdgeCount(), 0u);
    EXPECT_TRUE(cube.IsValid());
}

TEST(SoftBodyTest, PinVertex)
{
    auto cloth = SoftBody::CreateCloth(3, 3, 0.1f);

    EXPECT_FALSE(cloth.IsVertexPinned(0));

    cloth.PinVertex(0);
    EXPECT_TRUE(cloth.IsVertexPinned(0));
    EXPECT_FLOAT_EQ(cloth.invMasses[0], 0.0f);
}

TEST(SoftBodyTest, UnpinVertex)
{
    auto cloth = SoftBody::CreateCloth(3, 3, 0.1f);

    cloth.PinVertex(0);
    EXPECT_TRUE(cloth.IsVertexPinned(0));

    cloth.UnpinVertex(0, 2.0f);
    EXPECT_FALSE(cloth.IsVertexPinned(0));
    EXPECT_FLOAT_EQ(cloth.invMasses[0], 0.5f); // 1 / 2.0
}

TEST(SoftBodyTest, IsValidWithEmptyVertices)
{
    SoftBody body;
    EXPECT_FALSE(body.IsValid());
}

TEST(SoftBodyTest, IsValidWithMismatchedInvMasses)
{
    SoftBody body;
    body.vertices.push_back(glm::vec3(0.0f));
    body.vertices.push_back(glm::vec3(1.0f));
    body.invMasses.push_back(1.0f); // Missing one invMass

    EXPECT_FALSE(body.IsValid());
}

TEST(SoftBodyTest, IsValidWithMalformedFaces)
{
    SoftBody body;
    body.vertices.push_back(glm::vec3(0.0f));
    body.invMasses.push_back(1.0f);
    body.faces.push_back(0);
    body.faces.push_back(0); // Only 2 indices, not divisible by 3

    EXPECT_FALSE(body.IsValid());
}

TEST(SoftBodyTest, CreateFromMesh)
{
    std::vector<glm::vec3> verts = {
        glm::vec3(0, 0, 0),
        glm::vec3(1, 0, 0),
        glm::vec3(0.5f, 1, 0),
    };
    std::vector<uint32_t> faces = {0, 1, 2};

    auto body = SoftBody::CreateFromMesh(verts, faces);

    EXPECT_EQ(body.type, SoftBodyType::Custom);
    EXPECT_EQ(body.GetVertexCount(), 3u);
    EXPECT_EQ(body.GetFaceCount(), 1u);
    EXPECT_EQ(body.GetEdgeCount(), 3u); // 3 edges of triangle
    EXPECT_TRUE(body.IsValid());
}

TEST(SoftBodyTest, ClothVertexLayout)
{
    auto cloth = SoftBody::CreateCloth(4, 3, 0.5f); // 4 wide, 3 tall

    // First row
    EXPECT_EQ(cloth.vertices[0], glm::vec3(0.0f, 0.0f, 0.0f));
    EXPECT_EQ(cloth.vertices[1], glm::vec3(0.5f, 0.0f, 0.0f));
    EXPECT_EQ(cloth.vertices[2], glm::vec3(1.0f, 0.0f, 0.0f));
    EXPECT_EQ(cloth.vertices[3], glm::vec3(1.5f, 0.0f, 0.0f));

    // Second row
    EXPECT_EQ(cloth.vertices[4], glm::vec3(0.0f, 0.5f, 0.0f));
}

TEST(SoftBodyTest, RopeVertexLayout)
{
    auto rope = SoftBody::CreateRope(3, 0.2f);

    EXPECT_EQ(rope.GetVertexCount(), 4u);
    EXPECT_EQ(rope.vertices[0], glm::vec3(0.0f, 0.0f, 0.0f));
    EXPECT_EQ(rope.vertices[1], glm::vec3(0.0f, -0.2f, 0.0f));
    EXPECT_EQ(rope.vertices[2], glm::vec3(0.0f, -0.4f, 0.0f));
    EXPECT_EQ(rope.vertices[3], glm::vec3(0.0f, -0.6f, 0.0f));
}

//=============================================================================
// Integration Tests - SoftBody with Physics System
//=============================================================================

class SoftBodyIntegrationTest : public ::testing::Test {
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

TEST_F(SoftBodyIntegrationTest, CreateClothSoftBody)
{
    auto &registry = core->GetRegistry();

    Engine::Entity entity(core->CreateEntity());

    Object::Component::Transform transform;
    transform.SetPosition(glm::vec3(0.0f, 5.0f, 0.0f));
    registry.emplace<Object::Component::Transform>(entity, transform);

    // Create cloth and pin top corners
    auto cloth = SoftBody::CreateCloth(5, 5, 0.1f);
    cloth.PinVertex(0); // Top-left
    cloth.PinVertex(4); // Top-right
    registry.emplace<SoftBody>(entity, cloth);

    // Check SoftBodyInternal was created
    auto *internal = registry.try_get<SoftBodyInternal>(entity);
    ASSERT_NE(internal, nullptr);
    EXPECT_TRUE(internal->IsValid());
}

TEST_F(SoftBodyIntegrationTest, CreateRopeSoftBody)
{
    auto &registry = core->GetRegistry();

    Engine::Entity entity(core->CreateEntity());

    Object::Component::Transform transform;
    transform.SetPosition(glm::vec3(0.0f, 10.0f, 0.0f));
    registry.emplace<Object::Component::Transform>(entity, transform);

    auto rope = SoftBody::CreateRope(20, 0.05f);
    rope.PinVertex(0); // Pin top
    registry.emplace<SoftBody>(entity, rope);

    auto *internal = registry.try_get<SoftBodyInternal>(entity);
    ASSERT_NE(internal, nullptr);
    EXPECT_TRUE(internal->IsValid());
}

TEST_F(SoftBodyIntegrationTest, DestroySoftBody)
{
    auto &registry = core->GetRegistry();

    Engine::Entity entity(core->CreateEntity());

    Object::Component::Transform transform;
    registry.emplace<Object::Component::Transform>(entity, transform);

    auto cloth = SoftBody::CreateCloth(3, 3, 0.1f);
    registry.emplace<SoftBody>(entity, cloth);

    // Verify created
    ASSERT_NE(registry.try_get<SoftBodyInternal>(entity), nullptr);

    // Remove soft body
    registry.remove<SoftBody>(entity);

    // Verify internal was also removed
    EXPECT_EQ(registry.try_get<SoftBodyInternal>(entity), nullptr);
}

TEST_F(SoftBodyIntegrationTest, MultipleSoftBodies)
{
    auto &registry = core->GetRegistry();

    // Create multiple cloths
    for (int i = 0; i < 3; ++i)
    {
        Engine::Entity entity(core->CreateEntity());

        Object::Component::Transform transform;
        transform.SetPosition(glm::vec3(static_cast<float>(i) * 2.0f, 5.0f, 0.0f));
        registry.emplace<Object::Component::Transform>(entity, transform);

        auto cloth = SoftBody::CreateCloth(4, 4, 0.1f);
        cloth.PinVertex(0);
        cloth.PinVertex(3);
        registry.emplace<SoftBody>(entity, cloth);
    }

    // Count soft bodies
    int count = 0;
    registry.view<SoftBodyInternal>().each([&count](auto &) { ++count; });

    EXPECT_EQ(count, 3);
}

TEST_F(SoftBodyIntegrationTest, InvalidSoftBodyNotCreated)
{
    auto &registry = core->GetRegistry();

    Engine::Entity entity(core->CreateEntity());

    Object::Component::Transform transform;
    registry.emplace<Object::Component::Transform>(entity, transform);

    // Create invalid soft body (no vertices)
    SoftBody invalidBody;
    registry.emplace<SoftBody>(entity, invalidBody);

    // SoftBodyInternal should NOT be created
    auto *internal = registry.try_get<SoftBodyInternal>(entity);
    EXPECT_EQ(internal, nullptr);
}
