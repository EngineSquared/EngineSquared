#include "plugin/PluginCameraMovement.hpp"

#include "plugin/PluginInput.hpp"
#include "plugin/PluginWindow.hpp"
#include "resource/CameraManager.hpp"
#include "scheduler/Startup.hpp"
#include "system/CameraControlSystem.hpp"
#include "system/RegisterCameraCallbacksSystem.hpp"

namespace CameraMovement {

Plugin::Plugin(Engine::Core &core) : APlugin(core) {}

void Plugin::Bind()
{
    RequirePlugins<Input::Plugin, Window::Plugin>();

    RegisterResource(Resource::CameraManager(GetCore()));

    RegisterSystems<Engine::Scheduler::Startup>(System::RegisterCameraCallbacksSystem);
    RegisterSystems<Engine::Scheduler::Update>(System::CameraControlSystem);
}

} // namespace CameraMovement
