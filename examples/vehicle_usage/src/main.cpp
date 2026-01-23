/**************************************************************************
 * EngineSquared - Vehicle Usage Example
 *
 * This example demonstrates how to use the Graphic and Physics plugins together for vehicle simulation.
 **************************************************************************/

#include "Engine.hpp"

#include "CameraMovement.hpp"
#include "DefaultPipeline.hpp"
#include "Graphic.hpp"
#include "Input.hpp"
#include "Object.hpp"
#include "Physics.hpp"
#include "RenderingPipeline.hpp"
#include "plugin/PluginWindow.hpp"
#include "resource/Window.hpp"

#include "component/PlayerVehicle.hpp"
#include "resource/CameraControlSystemManager.hpp"
#include "scenes/VehicleScene.hpp"
#include "system/VehicleInput.hpp"
#include "utils/ChaseCameraBehavior.hpp"

void EscapeKeySystem(Engine::Core &core)
{
    auto &inputManager = core.GetResource<Input::Resource::InputManager>();

    if (inputManager.IsKeyPressed(GLFW_KEY_ESCAPE))
    {
        core.Stop();
    }
}

void Setup(Engine::Core &core)
{
    CreateCheckeredFloor(core);
    auto vehicle = CreateVehicle(core);

    auto camera = core.CreateEntity();

    camera.AddComponent<Object::Component::Transform>(core, glm::vec3(0.0f, 1.0f, -10.0f));
    camera.AddComponent<Object::Component::Camera>(core);

    auto &cameraManager = core.GetResource<CameraMovement::Resource::CameraManager>();
    cameraManager.SetActiveCamera(camera);
    cameraManager.SetMovementSpeed(3.0f);

    core.RegisterSystem(EscapeKeySystem);

    core.RegisterSystem<Engine::Scheduler::FixedTimeUpdate>(VehicleInput);

    auto chaseBehavior = std::make_shared<ChaseCameraBehavior>(vehicle);
    cameraManager.SetBehavior(chaseBehavior);

    auto &fixedTimeScheduler = core.GetScheduler<Engine::Scheduler::FixedTimeUpdate>();
    fixedTimeScheduler.SetTickRate(1.0f / 120.0f);

    auto &cameraControlSystemManager = core.GetResource<CameraMovement::Resource::CameraControlSystemManager>();
    cameraControlSystemManager.SetCameraControlSystemScheduler<Engine::Scheduler::FixedTimeUpdate>(core);
}

class GraphicExampleError : public std::runtime_error {
  public:
    using std::runtime_error::runtime_error;
};

int main(void)
{
    Engine::Core core;

    core.AddPlugins<Window::Plugin, DefaultPipeline::Plugin, Input::Plugin, CameraMovement::Plugin, Physics::Plugin>();

    core.RegisterSystem<Engine::Scheduler::Startup>(Setup);

    core.RunCore();

    return 0;
}
