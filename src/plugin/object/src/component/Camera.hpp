#pragma once

#include "Logger.hpp"
#include "component/Transform.hpp"
#include <entt/core/hashed_string.hpp>
#include <glm/glm.hpp>

namespace Object::Component {

struct Camera {
    using ID = entt::hashed_string;

    glm::mat4 projection;
    glm::mat4 view;
    glm::mat4 viewProjection;
    glm::mat4 inverseViewProjection;
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    float fov = glm::radians(70.0f);
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
    float aspectRatio = 1.f;

    void Update(const Object::Component::Transform &transform)
    {
        glm::vec3 forward = glm::normalize(transform.GetForwardVector() * transform.GetScale());
        view = glm::lookAtLH(transform.GetPosition(), transform.GetPosition() + forward, this->up);
        projection = glm::perspectiveLH_ZO(this->fov, aspectRatio, this->nearPlane, this->farPlane);
        viewProjection = projection * view;
        inverseViewProjection = glm::inverse(viewProjection);
    }

    void UpdateAspectRatio(const glm::uvec2 &textureSize)
    {
        if (textureSize.y > 0)
        {
            aspectRatio = static_cast<float>(textureSize.x) / static_cast<float>(textureSize.y);
        }
        else
        {
            Log::Warning("GPUCamera::UpdateAspectRatio: texture height is zero, cannot update aspect ratio.");
        }
    }
};

static_assert(sizeof(Camera) == sizeof(glm::mat4) * 4 + sizeof(glm::vec3) + sizeof(float) * 4);
} // namespace Object::Component
