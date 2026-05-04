#pragma once

#include <algorithm>
#include <cmath>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "component/Camera.hpp"
#include "component/Transform.hpp"
#include "core/Core.hpp"
#include "resource/CameraManager.hpp"
#include "resource/InputManager.hpp"
#include "resource/Window.hpp"
#include "utils/CameraBehavior.hpp"
#include "utils/CameraUtils.hpp"
#include "utils/InputUtils.hpp"

namespace CameraMovement::Utils {
class OrbitalChaseBehavior : public CameraMovement::Utils::ICameraBehavior {
  public:
    explicit OrbitalChaseBehavior(Engine::Core &core, Engine::Entity target) : _core(&core), _target(target)
    {
        if (!core.HasResource<Input::Resource::InputManager>())
        {
            Log::Warning("InputManager resource not found in core. OrbitalChaseBehavior will not function properly.");
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

    ~OrbitalChaseBehavior() override
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
            Log::Warning("Camera target entity is not alive. OrbitalChaseBehavior will not update.");
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
} // namespace CameraMovement::Utils
