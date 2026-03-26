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
#include "utils/CameraBehavior.hpp"

#include <iostream>

struct CharacterControllerExampleRuntime {
    Engine::Entity player;
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

class ThirdPlayerCameraBehavior : public CameraMovement::Utils::ICameraBehavior {
  public:
    explicit ThirdPlayerCameraBehavior(Engine::Core &core, Engine::Entity target) : _core(&core), _target(target)
    {
        if (!core.HasResource<Input::Resource::InputManager>())
        {
            Log::Warning(
                "InputManager resource not found in core. ThirdPlayerCameraBehavior will not function properly.");
            return;
        }

        auto &inputManager = core.GetResource<Input::Resource::InputManager>();

        _mouseButtonCallbackId =
            inputManager.RegisterMouseButtonCallback([this](Engine::Core &core, int button, int action, int mods) {
                HandleMouseButton(core, button, action, mods);
            });

        _cursorPosCallbackId = inputManager.RegisterCursorPosCallback(
            [this](Engine::Core &core, double xpos, double ypos) { HandleCursorPos(core, xpos, ypos); });

        _scrollCallbackId = inputManager.RegisterScrollCallback(
            [this](Engine::Core &core, double xoffset, double yoffset) { HandleScroll(core, xoffset, yoffset); });

        auto &cameraManager = core.GetResource<CameraMovement::Resource::CameraManager>();
        _lastMouseX = cameraManager.GetLastMouseX();
        _lastMouseY = cameraManager.GetLastMouseY();
    }

    ~ThirdPlayerCameraBehavior() override
    {
        if (!_core)
            return;

        try
        {
            if (_core->HasResource<Input::Resource::InputManager>())
            {
                auto &inputManager = _core->GetResource<Input::Resource::InputManager>();
                if (_mouseButtonCallbackId != 0)
                {
                    inputManager.DeleteMouseButtonCallback(_mouseButtonCallbackId);
                }
                if (_cursorPosCallbackId != 0)
                {
                    inputManager.DeleteCursorPosCallback(_cursorPosCallbackId);
                }
                if (_scrollCallbackId != 0)
                {
                    inputManager.DeleteScrollCallback(_scrollCallbackId);
                }
            }
        }
        catch (const std::exception &e)
        {
            Log::Error(fmt::format(
                "Failed to unregister camera behavior callbacks: {}",
                e.what())); // TODO: fix namming of log function (Warn rather than Warning or Err rather than Error)
        }
    }

    void Update(Engine::Core &core, CameraMovement::Resource::CameraManager &manager,
                Object::Component::Transform &cameraTransform, Object::Component::Camera &, float) override
    {
        auto &window = core.GetResource<Window::Resource::Window>();

        if (!_target.IsAlive())
        {
            Log::Warning("Camera target entity is not alive. ThirdPlayerCameraBehavior will not update.");
            return;
        }
        manager.SetWasCursorMasked(window.IsCursorMasked());

        this->UpdatePosition(cameraTransform);
        this->UpdateRotation(cameraTransform);
    }

    Engine::Entity GetTarget() const { return _target; }

    void SetTarget(Engine::Entity newTarget) { _target = newTarget; }

    float GetScrollSensitivity() const { return _scrollSensitivity; }

    void SetScrollSensitivity(float newScrollSensitivity) { _scrollSensitivity = newScrollSensitivity; }

    float GetMinDistance() const { return _minDistance; }

    void SetMinDistance(float newMinDistance) { _minDistance = newMinDistance; }

    float GetMaxDistance() const { return _maxDistance; }

    void SetMaxDistance(float newMaxDistance) { _maxDistance = newMaxDistance; }

  private:
    void UpdatePosition(Object::Component::Transform &cameraTransform)
    {
        auto &targetTransform = _target.GetComponents<Object::Component::Transform>();
        const auto &targetPos = targetTransform.GetPosition();
        const auto &targetRot = targetTransform.GetRotation();

        float horizontalDistance = _distance * std::cos(_pitch);
        glm::vec3 localOffset;
        localOffset.x = horizontalDistance * std::sin(_yaw);
        localOffset.y = _distance * std::sin(_pitch);
        localOffset.z = -horizontalDistance * std::cos(_yaw);

        glm::vec3 horizontalOffset = targetRot * glm::vec3(localOffset.x, 0.0f, localOffset.z);
        glm::vec3 cameraPosition = targetPos + horizontalOffset + glm::vec3(0.0f, localOffset.y, 0.0f);

        cameraTransform.SetPosition(cameraPosition);
    }

    void UpdateRotation(Object::Component::Transform &cameraTransform)
    {
        const auto &targetPosition = _target.GetComponents<Object::Component::Transform>().GetPosition();
        glm::quat lookRotation = CameraMovement::Utils::ComputeLookAtQuaternion(
            cameraTransform.GetPosition(), targetPosition, glm::vec3(0.0f, 1.0f, 0.0f));
        cameraTransform.SetRotation(lookRotation);
    }

    void HandleMouseButton(Engine::Core &, int button, int action, int)
    {
        if (button == GLFW_MOUSE_BUTTON_RIGHT)
        {
            _isDragging = (action == GLFW_PRESS);
        }
    }

    void HandleCursorPos(Engine::Core &core, float xpos, float ypos)
    {
        auto &cameraManager = core.GetResource<CameraMovement::Resource::CameraManager>();
        auto &window = core.GetResource<Window::Resource::Window>();

        bool shouldRotate =
            (window.IsCursorMasked() || _isDragging) && !(window.IsCursorMasked() && !cameraManager.WasCursorMasked());

        if (shouldRotate)
        {
            float sensitivity = cameraManager.GetMouseSensitivity() * 2.0f;

            float dx = xpos - _lastMouseX;
            float dy = ypos - _lastMouseY;

            _yaw -= dx * sensitivity;
            _pitch += dy * sensitivity;

            _yaw = glm::mod(_yaw, 2.0f * glm::pi<float>());
            _pitch = glm::clamp(_pitch, -glm::half_pi<float>() + 0.01f, glm::half_pi<float>() - 0.01f);
        }

        _lastMouseX = xpos;
        _lastMouseY = ypos;
    }

    void HandleScroll(Engine::Core &, double, double yoffset)
    {
        _distance -= yoffset * _scrollSensitivity;
        _distance = std::max(_minDistance, std::min(_maxDistance, _distance));
    }

    Engine::Core *_core;
    Engine::Entity _target;

    float _yaw = 0.0f;
    float _pitch = 0.35f;
    float _distance = 8.0f;
    float _minDistance = 0.001f;
    float _maxDistance = 50.0f;

    float _scrollSensitivity = 1.1f;

    bool _isDragging = false;
    float _lastMouseX = 0.0;
    float _lastMouseY = 0.0;

    FunctionUtils::FunctionID _mouseButtonCallbackId = 0;
    FunctionUtils::FunctionID _cursorPosCallbackId = 0;
    FunctionUtils::FunctionID _scrollCallbackId = 0;
};

void CharacterControllerInputSystem(Engine::Core &core)
{
    auto &runtime = core.GetResource<CharacterControllerExampleRuntime>();
    auto &cameraManager = core.GetResource<CameraMovement::Resource::CameraManager>();
    auto camera = cameraManager.GetActiveCamera();
    auto player = runtime.player;
    auto &inputManager = core.GetResource<Input::Resource::InputManager>();
    auto &controller = player.GetComponents<Physics::Component::CharacterController>();
    glm::vec2 movementInput{0.0f, 0.0f};

    if (inputManager.IsKeyPressed(GLFW_KEY_W))
        movementInput.y += 1.0f;
    if (inputManager.IsKeyPressed(GLFW_KEY_S))
        movementInput.y -= 1.0f;
    if (inputManager.IsKeyPressed(GLFW_KEY_A))
        movementInput.x -= 1.0f;
    if (inputManager.IsKeyPressed(GLFW_KEY_D))
        movementInput.x += 1.0f;

    if (movementInput.y != 0.0f && movementInput.x != 0.0f)
    {
        movementInput = glm::normalize(movementInput);
    }

    auto &cameraTransform = camera.GetComponents<Object::Component::Transform>();
    float cameraYaw = glm::atan(cameraTransform.GetRotation().y, cameraTransform.GetRotation().w) * 2.0f;
    float inputYaw = -cameraYaw;
    glm::vec2 rotatedInput;
    rotatedInput.x = movementInput.x * std::cos(inputYaw) - movementInput.y * std::sin(inputYaw);
    rotatedInput.y = movementInput.x * std::sin(inputYaw) + movementInput.y * std::cos(inputYaw);

    constexpr float moveSpeed = 3.5f;
    controller.linearVelocity.x = rotatedInput.x * moveSpeed;
    controller.linearVelocity.z = rotatedInput.y * moveSpeed;

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
    auto cameraBehavior = std::make_shared<ThirdPlayerCameraBehavior>(core, player);
    cameraManager.SetBehavior(cameraBehavior);

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
