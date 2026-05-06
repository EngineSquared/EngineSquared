#include "CameraMovement.hpp"
#include "DefaultPipeline.hpp"
#include "Engine.hpp"
#include "Graphic.hpp"
#include "Input.hpp"
#include "Object.hpp"
#include "RenderingPipeline.hpp"
#include "plugin/PluginWindow.hpp"
#include "resource/Window.hpp"
#include <set>
#include <string>

namespace Example {
enum class BehaviorType {
    Default,
    DontMove,
    OrbitalChase,
    AllTypes
};
struct CurrentCameraBehavior {
    BehaviorType current_behavior = BehaviorType::Default;
};

template <typename TType> inline TType Modulo(TType left, TType right)
{
    TType ret = left % right;
    return ret >= 0 ? ret : ret + right;
}

void Setup(Engine::Core &core)
{
    core.RegisterResource(CurrentCameraBehavior{});

    auto cube = core.CreateEntity();
    cube.AddComponent<Object::Component::Transform>();
    cube.AddComponent<Object::Component::Mesh>(Object::Utils::GenerateCubeMesh());

    auto camera = core.CreateEntity();
    camera.AddComponent<Object::Component::Transform>(glm::vec3(0.f, 1.0f, -2.5f), glm::vec3(1.0f),
                                                      glm::quat(glm::vec3(glm::radians(20.0f), 0.0f, 0.0f)));
    camera.AddComponent<Object::Component::Camera>();

    auto &cameraManager = core.GetResource<CameraMovement::Resource::CameraManager>();
    cameraManager.SetActiveCamera(camera);

    auto &inputManager = core.GetResource<Input::Resource::InputManager>();
    inputManager.RegisterKeyCallback([camera_cube = cube](Engine::Core &core, int key, int scancode, int action,
                                                          int mods) {
        if (action != GLFW_PRESS || (key != GLFW_KEY_UP && key != GLFW_KEY_DOWN))
        {
            return;
        }
        auto &currentCameraBehavior = core.GetResource<CurrentCameraBehavior>();
        auto &cameraManager = core.GetResource<CameraMovement::Resource::CameraManager>();
        int shift = 0;
        if (key == GLFW_KEY_UP)
        {
            shift = 1;
        }
        else if (key == GLFW_KEY_DOWN)
        {
            shift = -1;
        }
        else
        {
            return;
        }
        currentCameraBehavior.current_behavior =
            static_cast<BehaviorType>(Modulo((static_cast<int>(currentCameraBehavior.current_behavior) + shift),
                                             static_cast<int>(BehaviorType::AllTypes)));
        switch (currentCameraBehavior.current_behavior)
        {
        case BehaviorType::Default:
            cameraManager.SetBehavior(std::make_shared<CameraMovement::Utils::DefaultBehavior>(core));
            Log::Info("Switching to Default camera behavior. You can move with WASD keys and rotate with right click.");
            break;
        case BehaviorType::DontMove:
            cameraManager.SetBehavior(std::make_shared<CameraMovement::Utils::DontMoveBehavior>());
            Log::Info("Switching to DontMove camera behavior. You can't move and rotate.");
            break;
        case BehaviorType::OrbitalChase:
            cameraManager.SetBehavior(std::make_shared<CameraMovement::Utils::OrbitalChaseBehavior>(core, camera_cube));
            Log::Info("Switching to OrbitalChase camera behavior. You can't move but you can rotate with right key and "
                      "zoom using scroll wheel.");
            break;
        default: return;
        }
    });
    Log::Info("Press up and down arrows to switch camera behavior");
    Log::Info("Currently using Default camera behavior. You can move with WASD keys and rotate with right click.");
}

class GraphicExampleError : public std::runtime_error {
  public:
    using std::runtime_error::runtime_error;
};
} // namespace Example

int main(void)
{
    Engine::Core core;

    core.AddPlugins<Window::Plugin, DefaultPipeline::Plugin, CameraMovement::Plugin>();

    core.RegisterSystem<RenderingPipeline::Init>([](Engine::Core &core) {
        core.GetResource<Graphic::Resource::GraphicSettings>().SetOnErrorCallback(
            [](WGPUDevice const *, WGPUErrorType type, WGPUStringView message, WGPU_NULLABLE void *,
               WGPU_NULLABLE void *) {
                Log::Error(fmt::format("Custom uncaptured device error: type {:x} ({})", static_cast<uint32_t>(type),
                                       std::string(message.data, message.length)));
                throw Example::GraphicExampleError("Custom uncaptured device error occurred");
            });
    });

    core.RegisterSystem<Engine::Scheduler::Startup>(Example::Setup);

    core.Run();

    return 0;
}
