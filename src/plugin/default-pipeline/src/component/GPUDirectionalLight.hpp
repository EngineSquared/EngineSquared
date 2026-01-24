#pragma once

#include "Logger.hpp"
#include "component/DirectionalLight.hpp"
#include "glm/glm.hpp"
#include <entt/core/hashed_string.hpp>

namespace DefaultPipeline::Component {
struct GPUDirectionalLight {
    using Id = entt::hashed_string;

    glm::mat4 viewProjectionMatrix;
    Id buffer{};
    Id shadowTexture{};
    Id bindGroup{};

    void Update(const Object::Component::DirectionalLight &light, const Object::Component::Transform &transform)
    {
        glm::vec3 lightDirection = glm::normalize(light.direction);
        glm::vec3 posOfLight = transform.GetPosition();
        glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 0.1f, 60.0f);
        glm::vec3 target = posOfLight + glm::normalize(lightDirection) * 10.0f;
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

        glm::mat4 lightView = glm::lookAt(posOfLight, target, up);
        glm::mat4 lightSpaceMatrix = lightProjection * lightView;

        viewProjectionMatrix = lightSpaceMatrix;
    }
};
}; // namespace DefaultPipeline::Component
