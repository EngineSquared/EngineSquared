#pragma once

#include <GLFW/glfw3.h>
#include <memory>

#include "component/Camera.hpp"
#include "component/Transform.hpp"
#include "core/Core.hpp"
#include "entity/Entity.hpp"
#include "exception/CameraMovementError.hpp"

namespace CameraMovement::Component {
class ICameraBehavior;
}

namespace CameraMovement::Resource {

/**
 * @brief Enum to define the behavior type of the camera.
 */
enum class CameraBehaviorType {
    Default,
    DontMove,
};

/**
 * @brief CameraManager is a resource that manages the active camera entity.
 *
 * The camera entity must have both a Transform and a Camera component.
 * This resource is initialized with late binding - the camera entity is set after plugin binding.
 */
class CameraManager {
  public:
    explicit CameraManager(Engine::Core &core)
        : _core(core), _cameraEntity(Engine::Entity::entity_null_id),
          _behaviorType(CameraBehaviorType::Default),                                                    // NOSONAR
          _movementSpeed(5.0f), _mouseSensitivity(0.002f), _lastMouseX(0.0), _lastMouseY(0.0),           // NOSONAR
          _isMouseDragging(false),                                                                       // NOSONAR
          _wasCursorMasked(false), _originRotation(1.0f, 0.0f, 0.0f, 0.0f), _joystickId(GLFW_JOYSTICK_1) // NOSONAR
    {
        // sonar can't decide itself to choose whether it should be in initializer list or constructor body, so we
        // disable it
    }

    ~CameraManager() = default;

    /**
     * @brief Set the active camera entity.
     *
     * @param entity The entity to use as the camera.
     * @throws CameraMovementError if the entity is invalid or missing required components.
     */
    void SetActiveCamera(Engine::Entity entity)
    {
        if (!entity.IsValid())
        {
            throw CameraMovementError("Camera entity is invalid");
        }

        auto &registry = _core.GetRegistry();
        if (!registry.all_of<Object::Component::Transform, Object::Component::Camera>(entity))
        {
            throw CameraMovementError("Camera entity must have both Transform and Camera components");
        }

        _cameraEntity = entity;
    }

    /**
     * @brief Get the active camera entity.
     *
     * @return The active camera entity.
     * @throws CameraMovementError if the camera entity is not set or invalid.
     */
    Engine::Entity GetActiveCamera() const
    {
        if (_cameraEntity == Engine::Entity::entity_null_id)
        {
            throw CameraMovementError("Camera entity is not set");
        }

        if (!_cameraEntity.IsValid())
        {
            throw CameraMovementError("Camera entity is invalid");
        }

        auto &registry = _core.GetRegistry();
        if (!registry.all_of<Object::Component::Transform, Object::Component::Camera>(_cameraEntity))
        {
            throw CameraMovementError("Camera entity is missing required components");
        }

        return _cameraEntity;
    }

    /**
     * @brief Check if the camera entity is set and valid.
     *
     * @return True if the camera is set and valid, false otherwise.
     */
    bool HasValidCamera() const
    {
        if (_cameraEntity == Engine::Entity::entity_null_id)
        {
            return false;
        }

        if (!_cameraEntity.IsValid())
        {
            return false;
        }

        auto &registry = _core.GetRegistry();
        return registry.all_of<Object::Component::Transform, Object::Component::Camera>(_cameraEntity);
    }

    /**
     * @brief Set the behavior type of the camera.
     *
     * @param type The behavior type.
     */
    void SetBehaviorType(CameraBehaviorType type) { _behaviorType = type; }

    /**
     * @brief Get the behavior type of the camera.
     *
     * @return The behavior type.
     */
    CameraBehaviorType GetBehaviorType() const { return _behaviorType; }

    /**
     * @brief Set the movement speed of the camera.
     *
     * @param speed The movement speed.
     */
    void SetMovementSpeed(float speed) { _movementSpeed = speed; }

    /**
     * @brief Get the movement speed of the camera.
     *
     * @return The movement speed.
     */
    float GetMovementSpeed() const { return _movementSpeed; }

    /**
     * @brief Set the mouse sensitivity of the camera.
     *
     * @param sensitivity The mouse sensitivity.
     */
    void SetMouseSensitivity(float sensitivity) { _mouseSensitivity = sensitivity; }

    /**
     * @brief Get the mouse sensitivity of the camera.
     *
     * @return The mouse sensitivity.
     */
    float GetMouseSensitivity() const { return _mouseSensitivity; }

    /**
     * @brief Set the last mouse position.
     *
     * @param x The x position.
     * @param y The y position.
     */
    void SetLastMousePosition(double x, double y)
    {
        _lastMouseX = x;
        _lastMouseY = y;
    }

    /**
     * @brief Get the last mouse X position.
     *
     * @return The last mouse X position.
     */
    double GetLastMouseX() const { return _lastMouseX; }

    /**
     * @brief Get the last mouse Y position.
     *
     * @return The last mouse Y position.
     */
    double GetLastMouseY() const { return _lastMouseY; }

    /**
     * @brief Set whether the mouse is being dragged.
     *
     * @param dragging True if dragging, false otherwise.
     */
    void SetMouseDragging(bool dragging) { _isMouseDragging = dragging; }

    /**
     * @brief Check if the mouse is being dragged.
     *
     * @return True if dragging, false otherwise.
     */
    bool IsMouseDragging() const { return _isMouseDragging; }

    /**
     * @brief Set the origin rotation (used for mouse dragging).
     *
     * @param rotation The origin rotation.
     */
    void SetOriginRotation(const glm::quat &rotation) { _originRotation = rotation; }

    /**
     * @brief Get the origin rotation.
     *
     * @return The origin rotation.
     */
    const glm::quat &GetOriginRotation() const { return _originRotation; }

    /**
     * @brief Set whether the cursor was masked in the previous frame.
     *
     * @param masked True if cursor was masked, false otherwise.
     */
    void SetWasCursorMasked(bool masked) { _wasCursorMasked = masked; }

    /**
     * @brief Check if the cursor was masked in the previous frame.
     *
     * @return True if cursor was masked, false otherwise.
     */
    bool WasCursorMasked() const { return _wasCursorMasked; }

    /**
     * @brief Get the core reference.
     *
     * @return The core reference.
     */
    Engine::Core &GetCore() { return _core; }

    /**
     * @brief Set the joystick ID to use for camera control.
     *
     * @param joystickId The GLFW joystick ID (e.g., GLFW_JOYSTICK_1).
     */
    void SetJoystickId(int joystickId) { _joystickId = joystickId; }

    /**
     * @brief Get the joystick ID used for camera control.
     *
     * @return The GLFW joystick ID.
     */
    int GetJoystickId() const { return _joystickId; }

    /**
     * @brief Set the camera behavior instance.
     *
     * @param behavior Shared pointer to an implementation of ICameraBehavior.
     */
    void SetBehavior(std::shared_ptr<CameraMovement::Component::ICameraBehavior> behavior)
    {
        _behavior = std::move(behavior);
    }

    /**
     * @brief Get the current camera behavior instance.
     *
     * @return Shared pointer to the current ICameraBehavior, or nullptr if none is set.
     */
    std::shared_ptr<CameraMovement::Component::ICameraBehavior> GetBehavior() const { return _behavior; }

  private:
    Engine::Core &_core;
    Engine::Entity _cameraEntity;
    CameraBehaviorType _behaviorType;
    float _movementSpeed;
    float _mouseSensitivity;
    double _lastMouseX;
    double _lastMouseY;
    bool _isMouseDragging;
    bool _wasCursorMasked;
    glm::quat _originRotation;
    int _joystickId;
    std::shared_ptr<CameraMovement::Component::ICameraBehavior> _behavior;
};

} // namespace CameraMovement::Resource
