#pragma once

#include "glm/glm.hpp"
#include "system/initialization/CreateDefaultRenderPipeline.hpp"
#include "utils/DefaultPipeline.hpp"

namespace Graphic::Component {
struct GPUCamera {
    using Id = entt::hashed_string;
    // TODO: find a way to update this on window resize
    float aspectRatio = 800.0f / 800.0f;
    glm::mat4 projection;
    glm::mat4 view;
    glm::mat4 viewProjection;
    glm::mat4 inverseViewProjection;
    Id pipelineId = Graphic::Utils::DEFAULT_RENDER_GRAPH_ID;
};
}; // namespace Graphic::Component
