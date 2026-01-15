#include "system/CameraControlSystem.hpp"

#include "component/Camera.hpp"
#include "component/CameraBehavior.hpp"
#include "component/Transform.hpp"
#include "resource/CameraManager.hpp"
#include "scheduler/Update.hpp"

namespace CameraMovement::System {

void CameraControlSystem(Engine::Core &core)
{
    auto &cameraManager = core.GetResource<Resource::CameraManager>();

    if (!cameraManager.HasValidCamera())
    {
        return;
    }

    auto entity = cameraManager.GetActiveCamera();
    auto &transform = core.GetRegistry().get<Object::Component::Transform>(entity);
    auto &camera = core.GetRegistry().get<Object::Component::Camera>(entity);

    float deltaTime = core.GetScheduler<Engine::Scheduler::Update>().GetDeltaTime();

    auto behavior = cameraManager.GetBehavior();
    if (behavior)
    {
        behavior->Update(core, cameraManager, transform, camera, deltaTime);
    }
}

} // namespace CameraMovement::System
