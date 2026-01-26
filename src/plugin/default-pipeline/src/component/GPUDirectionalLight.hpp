#pragma once

#include "Logger.hpp"
#include "component/DirectionalLight.hpp"
#include "component/Transform.hpp"
#include "glm/glm.hpp"
#include "utils/webgpu.hpp"
#include <entt/core/hashed_string.hpp>

namespace DefaultPipeline::Component {
struct GPUDirectionalLight {
    using Id = entt::hashed_string;

    glm::mat4 viewProjectionMatrix;
    Id buffer{};
    Id bindGroupData{};
    wgpu::TextureView shadowTextureView;
    uint32_t shadowTextureIndex;

    void Update(const Object::Component::DirectionalLight &light, const Object::Component::Transform &transform)
    {
        // Why do we negate the forward vector?
        glm::vec3 lightDirection = -glm::normalize(transform.GetForwardVector() * transform.GetScale());
        glm::vec3 posOfLight = transform.GetPosition();
        glm::mat4 lightProjection = light.projection;
        glm::vec3 target = posOfLight + glm::normalize(lightDirection) * 10.0f;
        auto up = glm::vec3(0.0f, 1.0f, 0.0f);

        glm::mat4 lightView = glm::lookAt(posOfLight, target, up);
        glm::mat4 lightSpaceMatrix = lightProjection * lightView;

        viewProjectionMatrix = lightSpaceMatrix;
    }
};
}; // namespace DefaultPipeline::Component
