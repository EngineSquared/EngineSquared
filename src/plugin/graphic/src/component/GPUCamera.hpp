#pragma once

#include "component/Camera.hpp"
#include "component/Transform.hpp"
#include "glm/glm.hpp"
#include <entt/core/hashed_string.hpp>

namespace Graphic::Component {
struct GPUCamera {
    using Id = entt::hashed_string;
    // TODO: find a way to update this on window resize
    float aspectRatio = 1.f;
    glm::mat4 projection;
    glm::mat4 view;
    glm::mat4 viewProjection;
    glm::mat4 inverseViewProjection;
    Id buffer{};
    Id bindGroup{};
    Id pipeline{};

    void Update(const Object::Component::Camera &camera, const Object::Component::Transform &transform)
    {
        glm::vec3 forward = transform.GetRotation() * glm::vec3(0.0f, 0.0f, -1.0f);
        view = glm::lookAt(transform.GetPosition(), transform.GetPosition() + forward, camera.up);
        projection = glm::perspectiveLH_ZO(camera.fov, aspectRatio, camera.nearPlane, camera.farPlane);
        viewProjection = projection * view;
        inverseViewProjection = glm::inverse(viewProjection);
    }
};
}; // namespace Graphic::Component
