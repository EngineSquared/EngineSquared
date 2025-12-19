#include "system/GPUComponentManagement/OnCameraCreation.hpp"
#include "component/Camera.hpp"
#include "component/GPUCamera.hpp"
#include "component/Transform.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "system/initialization/CreateDefaultRenderPipeline.hpp"
#include "utils/DefaultPipeline.hpp"

void Graphic::System::OnCameraCreation(Engine::Core &core, Engine::Entity entity)
{
    auto &registry = core.GetRegistry();
    const auto &cameraComponent = entity.GetComponents<Object::Component::Camera>(core);
    const auto &transformComponent = entity.GetComponents<Object::Component::Transform>(core);

    Graphic::Component::GPUCamera gpuCameraComponent;
    glm::vec3 forward = transformComponent.rotation * glm::vec3(0.0f, 0.0f, 1.0f);
    gpuCameraComponent.view =
        glm::lookAt(transformComponent.position, transformComponent.position + forward, cameraComponent.up);
    gpuCameraComponent.projection = glm::perspectiveLH_ZO(cameraComponent.fov, gpuCameraComponent.aspectRatio,
                                                          cameraComponent.nearPlane, cameraComponent.farPlane);
    gpuCameraComponent.viewProjection = gpuCameraComponent.projection * gpuCameraComponent.view;
    gpuCameraComponent.inverseViewProjection = glm::inverse(gpuCameraComponent.viewProjection);
    gpuCameraComponent.pipelineId = Graphic::Utils::DEFAULT_RENDER_GRAPH_ID;

    entity.AddComponent<Graphic::Component::GPUCamera>(core, std::move(gpuCameraComponent));
}
