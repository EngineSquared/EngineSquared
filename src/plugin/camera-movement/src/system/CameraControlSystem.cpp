#include "system/CameraControlSystem.hpp"

#include "component/Camera.hpp"
#include "component/Transform.hpp"
#include "resource/CameraManager.hpp"
#include "scheduler/Update.hpp"
#include "utils/CameraBehavior.hpp"

namespace CameraMovement::System {

void CameraControlSystem(Engine::Core &core)
{
    auto &cameraManager = core.GetResource<Resource::CameraManager>();

    if (!cameraManager.HasValidCamera())
    {
        return;
    }

    auto entity = cameraManager.GetActiveCamera();
    auto &transform = entity.GetComponents<Object::Component::Transform>();
    auto &camera = entity.GetComponents<Object::Component::Camera>();

    float deltaTime = core.GetScheduler<Engine::Scheduler::Update>().GetDeltaTime();

    auto behavior = cameraManager.GetBehavior();
    if (behavior)
    {
        behavior->Update(core, cameraManager, transform, camera, deltaTime);
    }
}

} // namespace CameraMovement::System
