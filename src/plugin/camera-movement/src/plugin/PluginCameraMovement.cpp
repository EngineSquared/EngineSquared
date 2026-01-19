#include "plugin/PluginCameraMovement.hpp"

#include "plugin/PluginInput.hpp"
#include "plugin/PluginWindow.hpp"
#include "resource/CameraControlSystemManager.hpp"
#include "resource/CameraManager.hpp"
#include "scheduler/Startup.hpp"
#include "system/CameraControlSystem.hpp"
#include "utils/DefaultBehavior.hpp"
#include <memory>

namespace CameraMovement {

Plugin::Plugin(Engine::Core &core) : APlugin(core) {}

void Plugin::Bind()
{
    RequirePlugins<Input::Plugin, Window::Plugin>();

    RegisterResource(Resource::CameraManager(GetCore()));

    auto &cameraManager = GetCore().GetResource<Resource::CameraManager>();
    cameraManager.SetBehavior(std::make_shared<Utils::DefaultBehavior>(GetCore()));

    RegisterResource(Resource::CameraControlSystemManager(GetCore()));
}

} // namespace CameraMovement
