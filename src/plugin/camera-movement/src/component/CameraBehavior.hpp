#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "component/Camera.hpp"
#include "component/Transform.hpp"
#include "core/Core.hpp"
#include "resource/CameraManager.hpp"
#include "resource/InputManager.hpp"
#include "utils/CameraUtils.hpp"

namespace CameraMovement::Component {

/**
 * @brief Base interface for camera behaviors.
 *
 * Derived classes should implement the Update method to control camera movement.
 */
class ICameraBehavior {
  public:
    virtual ~ICameraBehavior() = default;

    /**
     * @brief Update the camera behavior.
     *
     * @param core The engine core.
     * @param manager The camera manager.
     * @param transform The transform component of the camera.
     * @param camera The camera component.
     * @param deltaTime The time elapsed since the last frame.
     */
    virtual void Update(Engine::Core &core, Resource::CameraManager &manager, Object::Component::Transform &transform,
                        Object::Component::Camera &camera, float deltaTime) = 0;
};

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
    ~DefaultBehavior() override = default;

    void Update(Engine::Core &core, Resource::CameraManager &manager, Object::Component::Transform &transform,
                Object::Component::Camera &camera, float deltaTime) override
    {
        auto &inputManager = core.GetResource<Input::Resource::InputManager>();

        HandleTranslation(inputManager, transform, manager.GetMovementSpeed(), deltaTime);
        HandleRotation(inputManager, transform, manager);
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

    void HandleRotation(Input::Resource::InputManager &inputManager, Object::Component::Transform &transform,
                        Resource::CameraManager &manager)
    {
        static bool callbacksRegistered = false;
        if (!callbacksRegistered)
        {
            inputManager.RegisterMouseButtonCallback([](Engine::Core &core, int button, int action, int mods) {
                if (button == GLFW_MOUSE_BUTTON_RIGHT)
                {
                    auto &cameraManager = core.GetResource<Resource::CameraManager>();
                    if (action == GLFW_PRESS)
                    {
                        cameraManager.SetMouseDragging(true);
                        auto entity = cameraManager.GetActiveCamera();
                        auto &transform = core.GetRegistry().get<Object::Component::Transform>(entity);
                        cameraManager.SetOriginRotation(transform.GetRotation());
                    }
                    else if (action == GLFW_RELEASE)
                    {
                        cameraManager.SetMouseDragging(false);
                    }
                }
            });

            inputManager.RegisterCursorPosCallback([](Engine::Core &core, double xpos, double ypos) {
                auto &cameraManager = core.GetResource<Resource::CameraManager>();
                if (cameraManager.IsMouseDragging() && cameraManager.HasValidCamera())
                {
                    double deltaX = xpos - cameraManager.GetLastMouseX();
                    double deltaY = ypos - cameraManager.GetLastMouseY();

                    float yaw = static_cast<float>(deltaX * cameraManager.GetMouseSensitivity());
                    float pitch = static_cast<float>(-deltaY * cameraManager.GetMouseSensitivity());

                    auto entity = cameraManager.GetActiveCamera();
                    auto &transform = core.GetRegistry().get<Object::Component::Transform>(entity);
                    glm::quat newRotation = Utils::RotateQuaternion(cameraManager.GetOriginRotation(), pitch, yaw);
                    transform.SetRotation(newRotation);

                    cameraManager.SetOriginRotation(newRotation);
                }
                cameraManager.SetLastMousePosition(xpos, ypos);
            });

            callbacksRegistered = true;
        }
    }
};

/**
 * @brief Camera behavior that does not move the camera.
 */
class DontMoveBehavior : public ICameraBehavior {
  public:
    DontMoveBehavior() = default;
    ~DontMoveBehavior() override = default;

    void Update(Engine::Core &core, Resource::CameraManager &manager, Object::Component::Transform &transform,
                Object::Component::Camera &camera, float deltaTime) override
    {
    }
};

} // namespace CameraMovement::Component
