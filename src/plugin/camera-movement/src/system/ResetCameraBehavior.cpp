#include "core/Core.hpp"
#include "utils/DontMoveBehavior.hpp"

namespace CameraMovement::System {

void ResetCameraBehavior(Engine::Core &core)
{
    auto &cameraManager = core.GetResource<Resource::CameraManager>();
    cameraManager.SetBehavior(std::make_shared<Utils::DontMoveBehavior>());
}

} // namespace CameraMovement::System
