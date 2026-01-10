#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

namespace CameraMovement::Utils {

/**
 * @brief Compute a quaternion that represents a "look at" rotation.
 *
 * @param eye The position of the camera.
 * @param target The point the camera is looking at.
 * @param up The up vector.
 * @return The quaternion representing the rotation.
 */
inline glm::quat ComputeLookAtQuaternion(const glm::vec3 &eye, const glm::vec3 &target, const glm::vec3 &up)
{
    glm::vec3 direction = glm::normalize(target - eye);
    glm::vec3 right = glm::normalize(glm::cross(up, direction));
    glm::vec3 newUp = glm::cross(direction, right);

    glm::mat3 rotationMatrix;
    rotationMatrix[0] = right;
    rotationMatrix[1] = newUp;
    rotationMatrix[2] = direction;

    return glm::quat_cast(rotationMatrix);
}

/**
 * @brief Apply pitch and yaw rotation to a quaternion.
 *
 * @param current The current rotation quaternion.
 * @param pitch The pitch angle in radians (rotation around right axis).
 * @param yaw The yaw angle in radians (rotation around up axis).
 * @return The new rotation quaternion.
 */
inline glm::quat RotateQuaternion(const glm::quat &current, float pitch, float yaw)
{
    glm::quat pitchQuat = glm::angleAxis(pitch, glm::vec3(1.0f, 0.0f, 0.0f));
    glm::quat yawQuat = glm::angleAxis(yaw, glm::vec3(0.0f, 1.0f, 0.0f));

    return glm::normalize(yawQuat * current * pitchQuat);
}

/**
 * @brief Get the right vector from a quaternion rotation.
 *
 * @param rotation The rotation quaternion.
 * @return The right vector.
 */
inline glm::vec3 GetRightVector(const glm::quat &rotation)
{
    return glm::normalize(rotation * glm::vec3(1.0f, 0.0f, 0.0f));
}

/**
 * @brief Get the up vector from a quaternion rotation.
 *
 * @param rotation The rotation quaternion.
 * @return The up vector.
 */
inline glm::vec3 GetUpVector(const glm::quat &rotation)
{
    return glm::normalize(rotation * glm::vec3(0.0f, 1.0f, 0.0f));
}

/**
 * @brief Get the forward vector from a quaternion rotation.
 *
 * @param rotation The rotation quaternion.
 * @return The forward vector.
 */
inline glm::vec3 GetForwardVector(const glm::quat &rotation)
{
    return glm::normalize(rotation * glm::vec3(0.0f, 0.0f, 1.0f));
}

} // namespace CameraMovement::Utils
