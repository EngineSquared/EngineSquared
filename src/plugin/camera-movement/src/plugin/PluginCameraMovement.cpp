#include "plugin/PluginCameraMovement.hpp"

#include <memory>
#include "plugin/PluginInput.hpp"
#include "plugin/PluginWindow.hpp"
#include "resource/CameraManager.hpp"
#include "component/DefaultBehavior.hpp"
#include "scheduler/Startup.hpp"
#include "system/CameraControlSystem.hpp"
#include "system/RegisterCameraCallbacksSystem.hpp"

namespace CameraMovement {

Plugin::Plugin(Engine::Core &core) : APlugin(core) {}

void Plugin::Bind()
{
    RequirePlugins<Input::Plugin, Window::Plugin>();

    RegisterResource(Resource::CameraManager(GetCore()));

    // Set a default behavior instance for the camera manager so the system can call Update on it directly.
    auto &cameraManager = GetCore().GetResource<Resource::CameraManager>();
    cameraManager.SetBehavior(std::make_shared<Component::DefaultBehavior>());

    RegisterSystems<Engine::Scheduler::Startup>(System::RegisterCameraCallbacksSystem);
    RegisterSystems<Engine::Scheduler::Update>(System::CameraControlSystem);
}

} // namespace CameraMovement
