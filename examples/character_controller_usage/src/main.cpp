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

#include <iostream>

struct CharacterControllerExampleRuntime {
    std::optional<Engine::Entity> player;
    bool jumpQueued = false;
};

void EscapeKeySystem(Engine::Core &core)
{
    auto &inputManager = core.GetResource<Input::Resource::InputManager>();
    inputManager.RegisterKeyCallback([&core](Engine::Core &, int key, int, int action, int) {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            core.Stop();
    });
}

void CharacterControllerInputSystem(Engine::Core &core)
{
    auto &runtime = core.GetResource<CharacterControllerExampleRuntime>();
    if (runtime.player.has_value() == false)
        return;
    auto player = runtime.player.value();

    auto &inputManager = core.GetResource<Input::Resource::InputManager>();
    auto &controller = player.GetComponents<Physics::Component::CharacterController>();

    float forward = 0.0f;
    float right = 0.0f;

    if (inputManager.IsKeyPressed(GLFW_KEY_W) || inputManager.IsKeyPressed(GLFW_KEY_Z))
        forward += 1.0f;
    if (inputManager.IsKeyPressed(GLFW_KEY_S))
        forward -= 1.0f;
    if (inputManager.IsKeyPressed(GLFW_KEY_A) || inputManager.IsKeyPressed(GLFW_KEY_Q))
        right -= 1.0f;
    if (inputManager.IsKeyPressed(GLFW_KEY_D))
        right += 1.0f;

    if (forward != 0.0f && right != 0.0f)
    {
        constexpr float diagonalScale = 0.70710677f; // 1 / sqrt(2)
        forward *= diagonalScale;
        right *= diagonalScale;
    }

    constexpr float moveSpeed = 3.5f;
    controller.linearVelocity.x = right * moveSpeed;
    controller.linearVelocity.z = forward * moveSpeed;

    const bool jumpPressed = inputManager.IsKeyPressed(GLFW_KEY_SPACE);
    if (jumpPressed && !runtime.jumpQueued)
    {
        controller.linearVelocity.y = 5.5f;
    }
    runtime.jumpQueued = jumpPressed;
}

void SetupCharacterControllerScene(Engine::Core &core)
{
    auto floor =
        Object::Helper::CreatePlane(core, {.width = 20.0f, .depth = 20.0f, .position = glm::vec3(0.0f, 0.0f, 0.0f)});
    floor.AddComponent<Physics::Component::BoxCollider>(glm::vec3(10.0f, 0.5f, 10.0f));
    floor.AddComponent<Physics::Component::RigidBody>(Physics::Component::RigidBody::CreateStatic());

    auto player =
        Object::Helper::CreateCapsule(core, {.radius = 0.3f, .height = 1.6f, .position = glm::vec3(0.0f, 4.0f, 0.0f)});
    player.AddComponent<Physics::Component::CapsuleCollider>(0.8f, 0.3f);
    player.AddComponent<Physics::Component::CharacterController>();

    auto camera = core.CreateEntity();
    camera.AddComponent<Object::Component::Transform>(glm::vec3(0.0f, 3.0f, -8.0f));
    camera.AddComponent<Object::Component::Camera>();

    auto pointLight = core.CreateEntity();
    pointLight.AddComponent<Object::Component::Transform>(glm::vec3(5.0f, 10.0f, -5.0f));
    pointLight.AddComponent<Object::Component::PointLight>(glm::vec3(0.8f, 0.8f, 0.8f), 1.f, 50.f, 0.1f);

    auto ambientLight = core.CreateEntity();
    ambientLight.AddComponent<Object::Component::AmbientLight>(glm::vec3(0.25f));

    auto &cameraManager = core.GetResource<CameraMovement::Resource::CameraManager>();
    cameraManager.SetActiveCamera(camera);
    cameraManager.SetMovementSpeed(4.0f);

    core.RegisterResource(CharacterControllerExampleRuntime{.player = player, .jumpQueued = false});

    core.RegisterSystem<Engine::Scheduler::Startup>(EscapeKeySystem);

    Log::Info("Controls: WASD/ZQSD to move, SPACE to jump, ESC to quit.");
}

class CharacterControllerExampleError : public std::runtime_error {
  public:
    using std::runtime_error::runtime_error;
};

int main(void)
{
    Engine::Core core;

    core.AddPlugins<Window::Plugin, DefaultPipeline::Plugin, Input::Plugin, CameraMovement::Plugin, Physics::Plugin>();

    core.RegisterSystem<RenderingPipeline::Init>([](Engine::Core &core) {
        core.GetResource<Graphic::Resource::GraphicSettings>().SetOnErrorCallback(
            [](WGPUDevice const *, WGPUErrorType type, WGPUStringView message, WGPU_NULLABLE void *,
               WGPU_NULLABLE void *) {
                Log::Error(fmt::format("Custom uncaptured device error: type {:x} ({})", static_cast<uint32_t>(type),
                                       std::string(message.data, message.length)));
                throw CharacterControllerExampleError("Custom uncaptured device error occurred");
            });
    });

    core.RegisterSystem<Engine::Scheduler::Startup>(SetupCharacterControllerScene);
    core.RegisterSystem<Engine::Scheduler::Update>(CharacterControllerInputSystem);

    core.RunCore();

    return 0;
}
