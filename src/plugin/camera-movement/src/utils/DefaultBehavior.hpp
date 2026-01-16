#pragma once

#include <cmath>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Logger.hpp"
#include "component/Camera.hpp"
#include "component/Transform.hpp"
#include "core/Core.hpp"
#include "resource/CameraManager.hpp"
#include "resource/InputManager.hpp"
#include "resource/Window.hpp"
#include "utils/CameraBehavior.hpp"
#include "utils/CameraUtils.hpp"

namespace CameraMovement::Utils {

/**
 * @brief Default camera behavior with WASD movement and mouse look.
 *
 * - WASD controls movement on X and Z axes
 * - Space/Shift controls movement on Y axis
 * - Right mouse button drag controls camera rotation
 */
class DefaultBehavior : public ICameraBehavior {
  public:
    DefaultBehavior() = default;

    /**
     * @brief Construct and register camera-related input callbacks.
     */
    explicit DefaultBehavior(Engine::Core &core) : _core(&core), _mouseButtonCallbackId(0), _cursorPosCallbackId(0)
    {
        if (!core.HasResource<Input::Resource::InputManager>())
        {
            Log::Warn("InputManager resource not found, cannot register camera callbacks");
            return;
        }

        auto &inputManager = core.GetResource<Input::Resource::InputManager>();
        _mouseButtonCallbackId = inputManager.RegisterMouseButtonCallback(MouseButtonCallback);
        _cursorPosCallbackId = inputManager.RegisterCursorPosCallback(CursorPosCallback);
    }

    ~DefaultBehavior() override
    {
        if (!_core)
        {
            return;
        }

        if (!_core->HasResource<Input::Resource::InputManager>())
        {
            return;
        }

        auto &inputManager = _core->GetResource<Input::Resource::InputManager>();
        if (_mouseButtonCallbackId != 0)
        {
            inputManager.DeleteMouseButtonCallback(_mouseButtonCallbackId);
        }
        if (_cursorPosCallbackId != 0)
        {
            inputManager.DeleteCursorPosCallback(_cursorPosCallbackId);
        }
    }

    void Update(Engine::Core &core, Resource::CameraManager &manager, Object::Component::Transform &transform,
                Object::Component::Camera &camera, float deltaTime) override
    {
        auto &inputManager = core.GetResource<Input::Resource::InputManager>();

        HandleTranslation(inputManager, transform, manager.GetMovementSpeed(), deltaTime);
        HandleJoystickInput(manager, transform, deltaTime);
    }

  private:
    void HandleTranslation(Input::Resource::InputManager &inputManager, Object::Component::Transform &transform,
                           float speed, float deltaTime)
    {
        glm::vec3 movement(0.0f);
        glm::vec3 forward = transform.GetForwardVector();
        glm::vec3 right = transform.GetRightVector();

        if (inputManager.IsKeyPressed(GLFW_KEY_W))
        {
            movement += forward;
        }
        if (inputManager.IsKeyPressed(GLFW_KEY_S))
        {
            movement -= forward;
        }
        if (inputManager.IsKeyPressed(GLFW_KEY_A))
        {
            movement -= right;
        }
        if (inputManager.IsKeyPressed(GLFW_KEY_D))
        {
            movement += right;
        }
        if (inputManager.IsKeyPressed(GLFW_KEY_SPACE))
        {
            movement += glm::vec3(0.0f, 1.0f, 0.0f);
        }
        if (inputManager.IsKeyPressed(GLFW_KEY_LEFT_SHIFT))
        {
            movement -= glm::vec3(0.0f, 1.0f, 0.0f);
        }

        if (glm::length(movement) > 0.0f)
        {
            movement = glm::normalize(movement);
            transform.SetPosition(transform.GetPosition() + movement * speed * deltaTime);
        }
    }

    void HandleJoystickInput(Resource::CameraManager &manager, Object::Component::Transform &transform, float deltaTime)
    {
        // As weird as it may seem, this is the correct mapping for a PS5 controller on GLFW
        constexpr int PS5_L3_LR_AXIS = 0;
        constexpr int PS5_L3_UD_AXIS = 1;
        constexpr int PS5_R3_LR_AXIS = 2;
        constexpr int PS5_L2_TRIGGER_AXIS = 3;
        constexpr int PS5_R2_TRIGGER_AXIS = 4;
        constexpr int PS5_R3_UD_AXIS = 5;
        constexpr float JOYSTICK_DEADZONE = 0.15f;
        constexpr float JOYSTICK_LOOK_SENSITIVITY = 2.0f;

        int joystickId = manager.GetJoystickId();

        if (!glfwJoystickPresent(joystickId))
        {
            return;
        }

        int axesCount = 0;
        const float *axes = glfwGetJoystickAxes(joystickId, &axesCount);

        if (axes == nullptr || axesCount < 6)
        {
            return;
        }

        glm::vec3 movement(0.0f);
        glm::vec3 forward = transform.GetForwardVector();
        glm::vec3 right = transform.GetRightVector();

        float horizontalAxis = axes[PS5_L3_LR_AXIS];
        float verticalAxis = axes[PS5_L3_UD_AXIS];
        float l2Trigger = axes[PS5_L2_TRIGGER_AXIS];
        float r2Trigger = axes[PS5_R2_TRIGGER_AXIS];

        if (std::abs(horizontalAxis) > JOYSTICK_DEADZONE)
        {
            movement += right * horizontalAxis;
        }
        if (std::abs(verticalAxis) > JOYSTICK_DEADZONE)
        {
            movement -= forward * verticalAxis;
        }

        float upMovement = (r2Trigger + 1.0f) / 2.0f;
        float downMovement = (l2Trigger + 1.0f) / 2.0f;

        if (upMovement > JOYSTICK_DEADZONE)
        {
            movement += glm::vec3(0.0f, 1.0f, 0.0f) * upMovement;
        }
        if (downMovement > JOYSTICK_DEADZONE)
        {
            movement -= glm::vec3(0.0f, 1.0f, 0.0f) * downMovement;
        }

        if (glm::length(movement) > 0.0f)
        {
            movement = glm::normalize(movement);
            transform.SetPosition(transform.GetPosition() + movement * manager.GetMovementSpeed() * deltaTime);
        }

        float lookHorizontal = axes[PS5_R3_LR_AXIS];
        float lookVertical = axes[PS5_R3_UD_AXIS];

        if (std::abs(lookHorizontal) <= JOYSTICK_DEADZONE)
        {
            lookHorizontal = 0.0f;
        }
        if (std::abs(lookVertical) <= JOYSTICK_DEADZONE)
        {
            lookVertical = 0.0f;
        }

        if (lookHorizontal != 0.0f || lookVertical != 0.0f)
        {
            float yaw = lookHorizontal * JOYSTICK_LOOK_SENSITIVITY * deltaTime;
            float pitch = lookVertical * JOYSTICK_LOOK_SENSITIVITY * deltaTime;

            glm::quat currentRotation = transform.GetRotation();
            glm::quat newRotation = Utils::RotateQuaternion(currentRotation, pitch, yaw);
            transform.SetRotation(newRotation);
        }
    }

  private:
    static void MouseButtonCallback(Engine::Core &core, int button, int action, int /* mods */)
    {
        if (button != GLFW_MOUSE_BUTTON_RIGHT)
        {
            return;
        }

        if (!core.GetRegistry().ctx().contains<Resource::CameraManager>())
        {
            Log::Warn("CameraManager resource not found in MouseButtonCallback");
            return;
        }

        auto &cameraManager = core.GetResource<Resource::CameraManager>();

        if (action == GLFW_PRESS)
        {
            cameraManager.SetMouseDragging(true);
            if (cameraManager.HasValidCamera())
            {
                auto entity = cameraManager.GetActiveCamera();
                auto &transform = core.GetRegistry().get<Object::Component::Transform>(entity);
                cameraManager.SetOriginRotation(transform.GetRotation());
            }
        }
        else if (action == GLFW_RELEASE)
        {
            cameraManager.SetMouseDragging(false);
        }
    }

    static void CursorPosCallback(Engine::Core &core, double xpos, double ypos)
    {
        if (!core.GetRegistry().ctx().contains<Resource::CameraManager>())
        {
            Log::Warn("CameraManager resource not found in CursorPosCallback");
            return;
        }

        if (!core.GetRegistry().ctx().contains<Window::Resource::Window>())
        {
            Log::Warn("Window resource not found in CursorPosCallback");
            return;
        }

        auto &cameraManager = core.GetResource<Resource::CameraManager>();
        auto &window = core.GetResource<Window::Resource::Window>();

        bool shouldRotate = (window.IsCursorMasked() || cameraManager.IsMouseDragging()) &&
                            cameraManager.HasValidCamera() &&
                            !(window.IsCursorMasked() && !cameraManager.WasCursorMasked());

        if (shouldRotate)
        {
            auto &transform = core.GetRegistry().get<Object::Component::Transform>(cameraManager.GetActiveCamera());
            auto yaw = static_cast<float>((xpos - cameraManager.GetLastMouseX()) * cameraManager.GetMouseSensitivity());
            auto pitch =
                static_cast<float>((ypos - cameraManager.GetLastMouseY()) * cameraManager.GetMouseSensitivity());

            glm::quat newRotation = Utils::RotateQuaternion(cameraManager.GetOriginRotation(), pitch, yaw);
            transform.SetRotation(newRotation);
            cameraManager.SetOriginRotation(newRotation);
        }

        cameraManager.SetLastMousePosition(xpos, ypos);
        cameraManager.SetWasCursorMasked(window.IsCursorMasked());
    }

    Engine::Core *_core = nullptr;
    FunctionUtils::FunctionID _mouseButtonCallbackId;
    FunctionUtils::FunctionID _cursorPosCallbackId;
};

} // namespace CameraMovement::Utils
