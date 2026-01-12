#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>
#include <gtest/gtest.h>

#include "utils/CameraUtils.hpp"

namespace {

constexpr float EPSILON = 0.0001f;

bool Vec3Equal(const glm::vec3 &a, const glm::vec3 &b, float epsilon = EPSILON)
{
    return glm::all(glm::epsilonEqual(a, b, epsilon));
}

bool QuatEqual(const glm::quat &a, const glm::quat &b, float epsilon = EPSILON)
{
    return glm::all(glm::epsilonEqual(glm::vec4(a.x, a.y, a.z, a.w), glm::vec4(b.x, b.y, b.z, b.w), epsilon)) ||
           glm::all(glm::epsilonEqual(glm::vec4(a.x, a.y, a.z, a.w), glm::vec4(-b.x, -b.y, -b.z, -b.w), epsilon));
}

} // namespace

TEST(CameraUtils, GetForwardVectorIdentity)
{
    auto identity = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 forward = CameraMovement::Utils::GetForwardVector(identity);

    EXPECT_TRUE(Vec3Equal(forward, glm::vec3(0.0f, 0.0f, 1.0f)));
}

TEST(CameraUtils, GetRightVectorIdentity)
{
    auto identity = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 right = CameraMovement::Utils::GetRightVector(identity);

    EXPECT_TRUE(Vec3Equal(right, glm::vec3(1.0f, 0.0f, 0.0f)));
}

TEST(CameraUtils, GetUpVectorIdentity)
{
    auto identity = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 up = CameraMovement::Utils::GetUpVector(identity);

    EXPECT_TRUE(Vec3Equal(up, glm::vec3(0.0f, 1.0f, 0.0f)));
}

TEST(CameraUtils, GetForwardVectorAfterYaw90)
{
    glm::quat yaw90 = glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 forward = CameraMovement::Utils::GetForwardVector(yaw90);

    EXPECT_TRUE(Vec3Equal(forward, glm::vec3(1.0f, 0.0f, 0.0f)));
}

TEST(CameraUtils, GetRightVectorAfterYaw90)
{
    glm::quat yaw90 = glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 right = CameraMovement::Utils::GetRightVector(yaw90);

    EXPECT_TRUE(Vec3Equal(right, glm::vec3(0.0f, 0.0f, -1.0f)));
}

TEST(CameraUtils, GetUpVectorAfterPitch90)
{
    glm::quat pitch90 = glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::vec3 up = CameraMovement::Utils::GetUpVector(pitch90);

    EXPECT_TRUE(Vec3Equal(up, glm::vec3(0.0f, 0.0f, 1.0f)));
}

TEST(CameraUtils, ComputeLookAtQuaternionLookingForward)
{
    glm::vec3 eye(0.0f, 0.0f, 0.0f);
    glm::vec3 target(0.0f, 0.0f, 1.0f);
    glm::vec3 up(0.0f, 1.0f, 0.0f);

    glm::quat lookAt = CameraMovement::Utils::ComputeLookAtQuaternion(eye, target, up);
    glm::vec3 forward = CameraMovement::Utils::GetForwardVector(lookAt);

    EXPECT_TRUE(Vec3Equal(forward, glm::vec3(0.0f, 0.0f, 1.0f)));
}

TEST(CameraUtils, ComputeLookAtQuaternionLookingRight)
{
    glm::vec3 eye(0.0f, 0.0f, 0.0f);
    glm::vec3 target(1.0f, 0.0f, 0.0f);
    glm::vec3 up(0.0f, 1.0f, 0.0f);

    glm::quat lookAt = CameraMovement::Utils::ComputeLookAtQuaternion(eye, target, up);
    glm::vec3 forward = CameraMovement::Utils::GetForwardVector(lookAt);

    EXPECT_TRUE(Vec3Equal(forward, glm::vec3(1.0f, 0.0f, 0.0f)));
}

TEST(CameraUtils, ComputeLookAtQuaternionLookingUp)
{
    glm::vec3 eye(0.0f, 0.0f, 0.0f);
    glm::vec3 target(0.0f, 1.0f, 0.0f);
    glm::vec3 up(0.0f, 0.0f, -1.0f);

    glm::quat lookAt = CameraMovement::Utils::ComputeLookAtQuaternion(eye, target, up);
    glm::vec3 forward = CameraMovement::Utils::GetForwardVector(lookAt);

    EXPECT_TRUE(Vec3Equal(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
}

TEST(CameraUtils, RotateQuaternionZeroPitchYaw)
{
    auto identity = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::quat rotated = CameraMovement::Utils::RotateQuaternion(identity, 0.0f, 0.0f);

    EXPECT_TRUE(QuatEqual(rotated, identity));
}

TEST(CameraUtils, RotateQuaternionYawOnly)
{
    auto identity = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::quat rotated = CameraMovement::Utils::RotateQuaternion(identity, 0.0f, glm::radians(90.0f));
    glm::vec3 forward = CameraMovement::Utils::GetForwardVector(rotated);

    EXPECT_TRUE(Vec3Equal(forward, glm::vec3(1.0f, 0.0f, 0.0f)));
}

TEST(CameraUtils, RotateQuaternionPitchOnly)
{
    auto identity = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::quat rotated = CameraMovement::Utils::RotateQuaternion(identity, glm::radians(90.0f), 0.0f);
    glm::vec3 forward = CameraMovement::Utils::GetForwardVector(rotated);

    EXPECT_TRUE(Vec3Equal(forward, glm::vec3(0.0f, -1.0f, 0.0f)));
}

TEST(CameraUtils, VectorsAreOrthogonal)
{
    glm::quat rotation = glm::angleAxis(glm::radians(30.0f), glm::normalize(glm::vec3(1.0f, 1.0f, 0.0f)));

    glm::vec3 forward = CameraMovement::Utils::GetForwardVector(rotation);
    glm::vec3 right = CameraMovement::Utils::GetRightVector(rotation);
    glm::vec3 up = CameraMovement::Utils::GetUpVector(rotation);

    EXPECT_NEAR(glm::dot(forward, right), 0.0f, EPSILON);
    EXPECT_NEAR(glm::dot(forward, up), 0.0f, EPSILON);
    EXPECT_NEAR(glm::dot(right, up), 0.0f, EPSILON);
}

TEST(CameraUtils, VectorsAreNormalized)
{
    glm::quat rotation = glm::angleAxis(glm::radians(45.0f), glm::normalize(glm::vec3(1.0f, 2.0f, 3.0f)));

    glm::vec3 forward = CameraMovement::Utils::GetForwardVector(rotation);
    glm::vec3 right = CameraMovement::Utils::GetRightVector(rotation);
    glm::vec3 up = CameraMovement::Utils::GetUpVector(rotation);

    EXPECT_NEAR(glm::length(forward), 1.0f, EPSILON);
    EXPECT_NEAR(glm::length(right), 1.0f, EPSILON);
    EXPECT_NEAR(glm::length(up), 1.0f, EPSILON);
}

TEST(CameraUtils, LookAtPreservesUpDirection)
{
    glm::vec3 eye(5.0f, 3.0f, 2.0f);
    glm::vec3 target(10.0f, 3.0f, 7.0f);
    glm::vec3 worldUp(0.0f, 1.0f, 0.0f);

    glm::quat lookAt = CameraMovement::Utils::ComputeLookAtQuaternion(eye, target, worldUp);
    glm::vec3 up = CameraMovement::Utils::GetUpVector(lookAt);

    EXPECT_GT(glm::dot(up, worldUp), 0.0f);
}
