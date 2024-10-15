#pragma once

#include <glm/glm.hpp>

namespace ES::Plugin::Camera::Component {
/**
 * Component used to store the 3D camera data.
 */
struct Camera3D {
    /**
     * Field used to store the camera lookAt position.
     */
    glm::vec3 lookAt;

    /**
     * Field used to store the camera up vector.
     */
    glm::vec3 up;

    /**
     * Field used to store the camera field of view.
     */
    float fov;

    /**
     * Field used to store the camera aspect ratio.
     */
    float aspect;

    /**
     * Field used to store the camera near plane.
     */
    float nearPlane;

    /**
     * Field used to store the camera far plane.
     */
    float farPlane;
};
} // namespace ES::Plugin::Camera::Component
