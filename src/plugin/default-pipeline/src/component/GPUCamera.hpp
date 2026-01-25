#pragma once

#include "Logger.hpp"
#include "component/Camera.hpp"
#include "component/Transform.hpp"
#include "glm/glm.hpp"
#include <entt/core/hashed_string.hpp>

namespace DefaultPipeline::Component {
struct GPUCamera {
    using Id = entt::hashed_string;

    float aspectRatio = 1.f;
    glm::mat4 projection;
    glm::mat4 view;
    glm::mat4 viewProjection;
    glm::mat4 inverseViewProjection;
    Id buffer{};
    Id bindGroup{};
    Id pipeline{};
    Id targetTexture{};

    void Update(const Object::Component::Camera &camera, const Object::Component::Transform &transform)
    {
        glm::vec3 forward = glm::normalize(transform.GetForwardVector() * transform.GetScale());
        view = glm::lookAtRH(transform.GetPosition(), transform.GetPosition() + forward, camera.up);
        projection = glm::perspectiveRH_ZO(camera.fov, aspectRatio, camera.nearPlane, camera.farPlane);
        viewProjection = projection * view;
        inverseViewProjection = glm::inverse(viewProjection);
    }

    inline void UpdateAspectRatio(const glm::uvec2 &textureSize)
    {
        if (textureSize.y > 0)
        {
            aspectRatio = static_cast<float>(textureSize.x) / static_cast<float>(textureSize.y);
        }
        else
        {
            Log::Warn("GPUCamera::UpdateAspectRatio: texture height is zero, cannot update aspect ratio.");
        }
    }
};
}; // namespace DefaultPipeline::Component
